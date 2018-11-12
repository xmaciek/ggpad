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

int GGPAD::exec()
{
    std::list<Gamepad*> list = m_deviceWatcher->newDevices();
    if ( list.empty() ) {
        return 0;
    }

    m_list.push_back( Binding() );
    m_list.back().first = list.front();
    m_list.back().second = new LuaScript();
    LuaScript& script = *m_list.back().second;
    script.bindTable( "Gamepad", GAMEPAD_TABLE );
    script.bindTable( "Keyboard", KEYBOARD_TABLE );
    script.bindTable( "Mouse", MOUSE_TABLE );

    typedef struct { const char* name; lua_CFunction func; } CB_REG;
    constexpr static const CB_REG CALLBACK_TABLE[] = {
        { "GGPAD_setKeyboard", &LuaBinder::facade<GGPAD::KbdFunc*,&GGPAD::setKeyboardState, int, bool> }
        , { "GGPAD_mouseMove", &LuaBinder::facade<GGPAD::MouseFunc*,&GGPAD::mouseMove, int, int> }
    };
    for ( const CB_REG& it : CALLBACK_TABLE ) {
        lua_register( script.vm(), it.name, it.func );
    }

    script.doFile( "test1.lua" );

    m_threadEvents = std::thread( &GGPAD::eventsLoop, this );
    m_threadUpdate = std::thread( &GGPAD::updateLoop, this );
    while ( m_isRunning ) {
        std::this_thread::sleep_for( std::chrono::seconds( 1 ) );
    }

    m_threadEvents.join();
    m_threadUpdate.join();
    return 0;
}

void GGPAD::eventsLoop()
{
    while ( m_isRunning ) {
        std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
        for ( Binding& binding : m_list ) {
            std::list<Gamepad::Event> events;
            Gamepad::Event e;
            while ( binding.first->pollChanges( &e ) ) {
                events.push_back( e );
            }
            for ( const Gamepad::Event& it : events ) {
                LuaScript::LockGuard lockGuard( *binding.second );
                binding.second->call( "GGPAD_buttonChanged" ) << it.button << it.value;
            }
        }
    }
}

void GGPAD::updateLoop()
{
    std::chrono::steady_clock::time_point lastUpdate = std::chrono::steady_clock::now();
    while ( m_isRunning ) {
        std::this_thread::sleep_for( std::chrono::milliseconds( 1 ) );
        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        const uint64_t d = std::chrono::duration_cast<std::chrono::milliseconds>( now - lastUpdate ).count();
        double deltaTime = (double)d / 1000;
        lastUpdate = now;
        for ( Binding& binding : m_list ) {
            LuaScript::LockGuard lockGuard( *binding.second );
            binding.second->call( "GGPAD_update" ) << deltaTime;
        }
    }
}

void GGPAD::setKeyboardState( uint32_t a_key, bool a_state )
{
    s_instance->m_systemEvent->keyboard( a_key, a_state );
}

void GGPAD::mouseMove( uint32_t a_key, int32_t a_state )
{
    s_instance->m_systemEvent->mouseMove( a_key, a_state );
}

