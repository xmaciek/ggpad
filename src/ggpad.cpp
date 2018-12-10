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
#include <thread>

#include "lua.hpp"

#include "ggpad.hpp"
#include "gamepad.hpp"
#include "lua_binder.hpp"
#include "watcher_udev.hpp"
#include "systemevent_linux.hpp"


static const std::vector<LuaScript::Record> GAMEPAD_TABLE {
    { "unknown", 0 }
#define MAKE_ENUM( NAME ) , { #NAME , Gamepad::NAME }
#include "button_enum.def"
#undef MAKE_ENUM
};

static const std::vector<LuaScript::Record> KEYBOARD_TABLE {
    { "unknown", 0 }
#define MAKE_ENUM( NAME, VALUE ) ,{ #NAME, KEY_ ## VALUE }
#include "key_enum.def"
#undef MAKE_ENUM
};

static const std::vector<LuaScript::Record> MOUSE_TABLE {
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
{
    if ( !s_instance ) {
        s_instance = this;
    }
    m_deviceWatcher = std::make_unique<WatcherUDev>();
    m_systemEvent = std::make_unique<SystemEventLinux>();
}

GGPAD::~GGPAD()
{
    m_isRunning = false;

    if ( s_instance == this ) {
        s_instance = nullptr;
    }
}

static void pushNewBinding( Gamepad* a_gamepad, std::list<std::unique_ptr<Binding>>* a_bindList )
{
    a_bindList->push_back( std::make_unique<Binding>() );
    a_bindList->back()->m_gamepad = a_gamepad;
    a_bindList->back()->m_script = new LuaScript();
    LuaScript& script = *a_bindList->back()->m_script;
    script.bindTable( "Gamepad", GAMEPAD_TABLE );
    script.bindTable( "Keyboard", KEYBOARD_TABLE );
    script.bindTable( "Mouse", MOUSE_TABLE );

    typedef struct { const char* name; lua_CFunction func; } CB_REG;
    constexpr static const CB_REG CALLBACK_TABLE[] = {
        { "GGPAD_keyboardSet", &LuaBinder::facade<GGPAD::KbdFunc*,&GGPAD::setKeyboard, int, bool> }
        , { "GGPAD_mouseMove", &LuaBinder::facade<GGPAD::MouseFunc*,&GGPAD::mouseMove, int, int> }
    };
    for ( const CB_REG& it : CALLBACK_TABLE ) {
        lua_register( script.vm(), it.name, it.func );
    }

    script.doFile( "test1.lua" );

    a_bindList->back()->m_hasUpdate = script.hasFunction( "GGPAD_update" );
    a_bindList->back()->m_hasEvent = script.hasFunction( "GGPAD_event" );
    a_bindList->back()->m_hasNativeEvent = script.hasFunction( "GGPAD_nativeEvent" );

    a_bindList->back()->run();
}

int GGPAD::exec()
{
    std::list<Gamepad*> list = m_deviceWatcher->currentDevices();
    for ( Gamepad* it : list ) {
        pushNewBinding( it, &m_list );
    }

    while ( m_isRunning ) {
        std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
        list = m_deviceWatcher->newDevices();
        for ( Gamepad* it : list ) {
            pushNewBinding( it, &m_list );
        }

        m_list.remove_if( Binding::isInvalid );
    }

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

