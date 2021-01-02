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

#include <algorithm>
#include <cassert>
#include <chrono>
#include <filesystem>
#include <thread>

#include "actuator.hpp"
#include "ggpad.hpp"
#include "gamepad.hpp"
#include "log.hpp"
#include "variant_index.hpp"
#include "table.hpp"


GGPAD* GGPAD::s_instance = nullptr;

GGPAD::GGPAD( Comm* clientComm )
: m_clientComm{ clientComm }
{
    if ( !s_instance ) {
        s_instance = this;
    }
    m_systemEvent = std::make_unique<SystemEvent>();
}

GGPAD::~GGPAD()
{
    m_isRunning = false;

    if ( s_instance == this ) {
        s_instance = nullptr;
    }
}

static Binding* bindingForGamepad( Gamepad* a_gamepad, std::list<Binding>* a_bindList )
{
    for ( Binding& it : *a_bindList ) {
        if ( it.m_gamepadId == a_gamepad->uid() ) {
            return &it;
        }
    }
    a_bindList->emplace_back();
    return &a_bindList->back();

}

static void setScriptForGamepad( Binding* binding, const std::filesystem::path& a_scriptFile )
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
    script->bindTable( "Gamepad"sv, std::begin( g_tableGamepad ), std::end( g_tableGamepad ) );
    script->bindTable( "Keyboard"sv, std::begin( g_tableKeyboard ), std::end( g_tableKeyboard ) );
    script->bindTable( "Mouse"sv, std::begin( g_tableMouse ), std::end( g_tableMouse ) );

    script->registerFunction( "GGPAD_keyboardSet", &Script::facade<decltype(&GGPAD::setKeyboard), &GGPAD::setKeyboard, int, bool> );
    script->registerFunction( "GGPAD_mouseMove",   &Script::facade<decltype(&GGPAD::mouseMove), &GGPAD::mouseMove, int, int> );
    script->registerFunction( "GGPAD_mouseSet", &Script::facade<decltype(&GGPAD::mouseSet), &GGPAD::mouseSet, int, int> );

    script->doFile( a_scriptFile.c_str() );

    binding->stopScript();
    binding->setCurrentScriptFile( a_scriptFile );
    binding->setScript( script );
    binding->discardEventQueue();
}

static void pushNewBinding( Gamepad* a_gamepad, std::list<Binding>* a_bindList, const std::filesystem::path& a_scriptFile )
{
    Binding* ptr = bindingForGamepad( a_gamepad, a_bindList );
    ptr->setGamepad( a_gamepad );
    setScriptForGamepad( ptr, a_scriptFile );
    ptr->run();
}

void GGPAD::quit()
{
    m_isRunning = false;
    m_settings.save();
}

static Binding* findBindingForId( std::list<Binding>& list, int32_t id )
{
    auto it = std::find_if( list.begin(), list.end(),
        [id]( const Binding& b )
        {
            return b.gamepadId() == id;
        }
    );
    return it != list.end() ? &(*it) : nullptr;
}

int GGPAD::exec()
{
    m_threadClientMessages = std::thread( &GGPAD::processClientMessages, this );

    while ( m_isRunning ) {
        std::optional<Gamepad::RuntimeId> toDisconnect{};
        SDLApp::Event event = m_sdlApp.next();

        switch ( event.index() ) {
        case indexOf<SDLApp::Event, std::monostate>():
            break;

        case indexOf<SDLApp::Event, SDLApp::SleepFor>():
            std::this_thread::sleep_for( std::get<SDLApp::SleepFor>( event ).value );
            m_sdlApp.update();
            break;

        case indexOf<SDLApp::Event, SDLApp::Connected>(): {
            SDLApp::Connected& connected = std::get<SDLApp::Connected>( event );
            LOG( LOG_DEBUG, "Connecting %08lX", connected.gamepad->uid() );
            pushNewBinding( connected.gamepad, &m_list, m_settings[ connected.gamepad->uid() ] );
            m_clientComm->pushToClient( Message{
                Message::Type::eGamepadConnected,
                connected.gamepad->uid(),
                connected.gamepad->displayName()
            } );
            m_clientComm->pushToClient( Message{
                Message::Type::eUpdateScriptPath,
                connected.gamepad->uid(),
                m_settings[ connected.gamepad->uid() ]
            } );
        } break;

        case indexOf<SDLApp::Event, SDLApp::Disconnected>():
            toDisconnect = std::get<SDLApp::Disconnected>( event ).id;
            LOG( LOG_DEBUG, "Disconnecting %u", toDisconnect->value );
            break;

        case indexOf<SDLApp::Event, SDLApp::Input>(): {
            const SDLApp::Input& input = std::get<SDLApp::Input>( event );
            Binding* binding = findByRuntimeId( input.id );
            if ( binding ) {
                binding->pushEvent( input.value );
            }
        } break;
        }

        for ( Binding& it : m_list ) {
            if ( toDisconnect && *toDisconnect == it.gamepadRuntimeId() ) {
                it.disconnect();
            }
            const bool stateChanged = it.connectionStateChanged();
            it.scriptStateChanged();
            if ( stateChanged && !it.connectionState() ) {
                m_clientComm->pushToClient( Message{ Message::Type::eGamepadDisconnected, it.m_gamepadId } );
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
                runScript( msg->m_id, msg->toPath() );
                break;

            case Message::Type::eStopScript:
                stopScript( msg->m_id );
                break;

            case Message::Type::eUpdateScriptPath:
                m_settings[ msg->m_id ] = msg->toPath();
                break;

            default:
                assert( !"unhandled message enum" );
            }
        }
        m_clientComm->waitForClient( std::chrono::milliseconds( 100 ) );
    }
}

void GGPAD::setKeyboard( uint32_t a_key, bool a_state )
{
    assert( s_instance );
    assert( s_instance->m_systemEvent );
    s_instance->m_systemEvent->keyboard( a_key, a_state );
}

void GGPAD::mouseMove( int32_t x, int32_t y )
{
    assert( s_instance );
    assert( s_instance->m_systemEvent );
    s_instance->m_systemEvent->mouseMove( x, y );
}

void GGPAD::mouseSet( int32_t x, int32_t y )
{
    assert( s_instance );
    assert( s_instance->m_systemEvent );
    s_instance->m_systemEvent->mouseSet( x, y );
}

Binding* GGPAD::findById( uint64_t id )
{
    for ( Binding& it : m_list ) {
        if ( it.m_gamepadId == id ) {
            return &it;
        }
    }
    LOG( LOG_ERROR, "Failed to find binding for gamepad id %lu", id );
    return nullptr;
}

Binding* GGPAD::findByRuntimeId( Gamepad::RuntimeId rid )
{
    auto it = std::find_if( m_list.begin(), m_list.end(),
        [rid]( const Binding& b )
        {
            return b.gamepadRuntimeId() == rid;
        }
    );
    return it != m_list.end() ? &*it : nullptr;
}

void GGPAD::runScript( uint64_t id, const std::filesystem::path& path )
{
    LOG( LOG_DEBUG, "%s\n", __FUNCTION__ );

    Binding* binding = findById( id );
    assert( binding );
    if ( !binding ) {
        LOG( LOG_ERROR, "Failed to find binding for gamepad id %lu", id );
        return;
    }
    setScriptForGamepad( binding, path );
    m_settings[ id ] = path;
    binding->run();
}

void GGPAD::stopScript( uint64_t id )
{
    LOG( LOG_DEBUG, "%s\n", __FUNCTION__ );

    Binding* binding = findById( id );
    assert( binding );
    if ( !binding ) {
        LOG( LOG_ERROR, "Failed to find binding for gamepad id %lu", id );
        return;
    }
    binding->stopScript();
}
