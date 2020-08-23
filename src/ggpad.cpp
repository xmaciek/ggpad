// GGPAD Copyright 2018 Maciej Latocha
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include <cassert>
#include <chrono>
#include <filesystem>
#include <thread>

#include "ggpad.hpp"
#include "gamepad.hpp"
#include "log.hpp"
#include "watcher_udev.hpp"


static const std::vector<Script::Record> GAMEPAD_TABLE {
    { "unknown", 0 }
#define MAKE_ENUM( NAME ) , { #NAME , Gamepad::NAME }
#include "button_enum.def"
#undef MAKE_ENUM
};

static const std::vector<Script::Record> KEYBOARD_TABLE {
    { "unknown", 0 }
#define MAKE_ENUM( NAME, VALUE ) ,{ #NAME, KEY_ ## VALUE }
#include "key_enum.def"
#undef MAKE_ENUM
};

static const std::vector<Script::Record> MOUSE_TABLE {
    { "unknown", 0 }
#define MAKE_ENUM( NAME, VALUE ) ,{ #NAME, BTN_ ## VALUE }
#define MAKE_ENUM2( NAME, VALUE ) ,{ #NAME, REL_ ## VALUE }
#include "mouse_enum.def"
#undef MAKE_ENUM2
#undef MAKE_ENUM
};

GGPAD* GGPAD::s_instance = nullptr;

GGPAD::GGPAD( Comm* clientComm )
: m_clientComm{ clientComm }
{
    if ( !s_instance ) {
        s_instance = this;
    }
    m_gui = std::make_unique<Gui>( clientComm );
    m_systemEvent = std::make_unique<SystemEvent>();
    m_deviceWatcher = std::make_unique<WatcherUDev>();
}

GGPAD::~GGPAD()
{
    m_isRunning = false;

    if ( s_instance == this ) {
        s_instance = nullptr;
    }
}

static Binding::Ptr& bindingForGamepad( Gamepad* a_gamepad, GGPAD::BindList* a_bindList )
{
    for ( Binding::Ptr& it : *a_bindList ) {
        if ( it->m_gamepadId == a_gamepad->uid() ) {
            return it;
        }
    }
    a_bindList->push_back( std::make_unique<Binding>() );
    return a_bindList->back();

}

static void setScriptForGamepad( Binding* binding, const std::string& a_scriptFile )
{
    if ( !binding ) {
        LOG( LOG_DEBUG, "Gamepad not selected" );
        return;
    }

    if ( !std::filesystem::exists( a_scriptFile ) ) {
        LOG( LOG_ERROR, "Script file not found: %s", a_scriptFile.c_str() );
        return;
    }

    LOG( LOG_DEBUG, "Opening file \"%s\"", a_scriptFile.c_str() );
    Script* script = new Script();
    script->bindTable( "Gamepad", GAMEPAD_TABLE );
    script->bindTable( "Keyboard", KEYBOARD_TABLE );
    script->bindTable( "Mouse", MOUSE_TABLE );

    script->registerFunction( "GGPAD_keyboardSet", &Script::facade<decltype(&GGPAD::setKeyboard), &GGPAD::setKeyboard, int, bool> );
    script->registerFunction( "GGPAD_mouseMove",   &Script::facade<decltype(&GGPAD::mouseMove), &GGPAD::mouseMove, int, int> );

    script->doFile( a_scriptFile.c_str() );

    binding->stopScript();
    binding->setCurrentScriptFile( a_scriptFile );
    binding->setScript( script );
    binding->discardEventQueue();
}

static void pushNewBinding( Gamepad* a_gamepad, GGPAD::BindList* a_bindList, const std::string& a_scriptFile )
{
    Binding::Ptr& ptr = bindingForGamepad( a_gamepad, a_bindList );
    ptr->setGamepad( a_gamepad );
    setScriptForGamepad( ptr.get(), a_scriptFile );
    ptr->run();
}

void GGPAD::quit()
{
    m_isRunning = false;
}

int GGPAD::exec()
{
    m_threadClientMessages = std::thread( &GGPAD::processClientMessages, this );
    std::list<Gamepad*> list = m_deviceWatcher->currentDevices();
    for ( Gamepad* it : list ) {
        pushNewBinding( it, &m_list, m_settings[ it->uid() ] );
        m_clientComm->pushToClient( Message{
            m_settings[ it->uid() ]
            , it->displayName()
            , it->uid()
            , Message::Type::eGamepadConnected
            }
        );

    }

    while ( m_isRunning ) {
        std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
        list = m_deviceWatcher->newDevices();
        for ( Gamepad* it : list ) {
            pushNewBinding( it, &m_list, m_settings[ it->uid() ] );
            m_clientComm->pushToClient( Message{
                m_settings[ it->uid() ]
                , it->displayName()
                , it->uid()
                , Message::Type::eGamepadConnected
                }
            );
        }

        for ( Binding::Ptr& ptr : m_list ) {
            const bool stateChanged = ptr->connectionStateChanged();
            // delay in gui will be visible since update goes once per second
            // TODO: tell gui script has stopped
            ptr->scriptStateChanged();
            if ( stateChanged && !ptr->connectionState() ) {
                m_clientComm->pushToClient( Message{ {}, {}, ptr->m_gamepadId, Message::Type::eGamepadDisconnected } );
            }
        }
    }

    if ( m_threadClientMessages.joinable() ) {
        m_threadClientMessages.join();
    }

    m_list.clear();
    return 0;
}

void GGPAD::processClientMessages()
{
    assert( m_clientComm );
    while ( m_isRunning ) {
        while ( std::optional<Message> msg = m_clientComm->popFromClient() ) {
            switch ( msg->m_type ) {
            case Message::Type::eRunScript:
                runScript( msg->m_id, msg->m_path );
                break;

            case Message::Type::eStopScript:
                stopScript( msg->m_id );
                break;

            default:
                assert( !"unhandled message enum" );
            }
        }
        std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
    }
}

void GGPAD::setKeyboard( uint32_t a_key, bool a_state )
{
    s_instance->m_systemEvent->keyboard( a_key, a_state );
}

void GGPAD::mouseMove( uint32_t a_key, int32_t a_state )
{
    s_instance->m_systemEvent->mouseMove( a_key, a_state );
}

Binding* GGPAD::findById( uint64_t id )
{
    for ( Binding::Ptr& it : m_list ) {
        assert( it );
        if ( it->m_gamepadId == id ) {
            return it.get();
        }
    }
    LOG( LOG_ERROR, "Failed to find binding for gamepad id %llu", id );
    return nullptr;
}

void GGPAD::runScript( uint64_t id, const std::filesystem::path& path )
{
    LOG( LOG_DEBUG, "%s\n", __FUNCTION__ );

    Binding* binding = findById( id );
    assert( binding );
    if ( !binding ) {
        LOG( LOG_ERROR, "Failed to find binding for gamepad id %llu", id );
        return;
    }
    setScriptForGamepad( binding, path.native() );
}

void GGPAD::stopScript( uint64_t id )
{
    LOG( LOG_DEBUG, "%s\n", __FUNCTION__ );

    Binding* binding = findById( id );
    assert( binding );
    if ( !binding ) {
        LOG( LOG_ERROR, "Failed to find binding for gamepad id %llu", id );
        return;
    }
    binding->stopScript();
}
