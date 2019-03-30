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

#include <chrono>
#include <filesystem>
#include <thread>

#include "ggpad.hpp"
#include "gamepad.hpp"
#include "gui_controller_model.hpp"
#include "log.hpp"
#include "watcher_udev.hpp"
#include "systemevent_linux.hpp"


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

GGPAD::GGPAD()
: m_isRunning( true )
, m_guiModel( &m_bindingMutex, &m_list )
{
    if ( !s_instance ) {
        s_instance = this;
    }
    m_deviceWatcher = std::make_unique<WatcherUDev>();
    m_systemEvent = std::make_unique<SystemEventLinux>();
    m_gui = std::make_unique<Gui>( &m_guiModel );
}

GGPAD::~GGPAD()
{
    m_isRunning = false;

    if ( s_instance == this ) {
        s_instance = nullptr;
    }
}

static void pushNewBinding( Gamepad* a_gamepad, GGPAD::BindList* a_bindList, const std::string& a_scriptFile )
{
    a_bindList->push_back( std::make_unique<Binding>() );
    a_bindList->back()->m_gamepad = a_gamepad;
    if ( !std::filesystem::exists( a_scriptFile ) ) {
        return;
    }

    a_bindList->back()->m_script = new Script();
    Script& script = *a_bindList->back()->m_script;
    script.bindTable( "Gamepad", GAMEPAD_TABLE );
    script.bindTable( "Keyboard", KEYBOARD_TABLE );
    script.bindTable( "Mouse", MOUSE_TABLE );

    script.registerFunction( "GGPAD_keyboardSet", &Script::facade<decltype(&GGPAD::setKeyboard), &GGPAD::setKeyboard, int, bool> );
    script.registerFunction( "GGPAD_mouseMove",   &Script::facade<decltype(&GGPAD::mouseMove), &GGPAD::mouseMove, int, int> );

    script.doFile( a_scriptFile.c_str() );

    a_bindList->back()->m_hasUpdate = script.hasFunction( "GGPAD_update" );
    a_bindList->back()->m_hasEvent = script.hasFunction( "GGPAD_event" );
    a_bindList->back()->m_hasNativeEvent = script.hasFunction( "GGPAD_nativeEvent" );

    a_bindList->back()->run();
}

void GGPAD::quit()
{
    m_isRunning = false;
}

int GGPAD::exec()
{
    bool dirty = false;
    std::list<Gamepad*> list = m_deviceWatcher->currentDevices();
    for ( Gamepad* it : list ) {
        dirty = true;
        std::lock_guard<std::mutex> lg( m_bindingMutex );
        pushNewBinding( it, &m_list, m_config[ it->uid() ] );
    }

    if ( dirty ) {
        m_guiModel.refreshViews();
    }

    while ( m_isRunning ) {
        dirty = false;
        std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
        list = m_deviceWatcher->newDevices();
        for ( Gamepad* it : list ) {
            dirty = true;
            std::lock_guard<std::mutex> lg( m_bindingMutex );
            pushNewBinding( it, &m_list, m_config[ it->uid() ] );
        }

        {
            const std::size_t size = m_list.size();
            std::lock_guard<std::mutex> lg( m_bindingMutex );
            m_list.erase( std::remove_if( m_list.begin(), m_list.end(), Binding::isInvalid ), m_list.end() );
            dirty |= size != m_list.size();
        }
        if ( dirty ) {
            LOG( LOG_DEBUG, "Changing dev listing\n" );
            m_guiModel.refreshViews();
        }
    }

    std::lock_guard<std::mutex> lg( m_bindingMutex );
    m_list.clear();

    return 0;
}

void GGPAD::setKeyboard( uint32_t a_key, bool a_state )
{
    s_instance->m_systemEvent->keyboard( a_key, a_state );
}

void GGPAD::mouseMove( uint32_t a_key, int32_t a_state )
{
    s_instance->m_systemEvent->mouseMove( a_key, a_state );
}

