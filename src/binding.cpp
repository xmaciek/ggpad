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
#include <list>

#include "binding.hpp"

Binding::Binding()
: m_hasUpdate( false )
, m_hasEvent( false )
, m_hasNativeEvent( false )
, m_isRunning( false )
, m_gamepadId( 0 )
, m_gamepad( nullptr )
, m_script( nullptr )
{
}

Binding::~Binding()
{
    bool active = m_isRunning;
    m_isRunning = false;
    if ( active ) {
        m_eventThread.join();
        m_updateThread.join();
    }
    delete m_gamepad;
    delete m_script;
}

void Binding::run()
{
    m_isRunning = true;
    m_eventThread = std::thread( &Binding::eventLoop, this );
    m_updateThread = std::thread( &Binding::updateLoop, this );
}

void Binding::updateLoop()
{
    if ( !m_hasUpdate ) {
        return;
    }
    assert( m_script );
    const double deltaTime = 5.0 / 1000;
    while ( m_isRunning ) {
        std::this_thread::sleep_for( std::chrono::milliseconds( 5 ) );
        LockGuard lockGuard( m_mutex );
        m_script->call( "GGPAD_update" ) << deltaTime;
    }
}

void Binding::eventLoop()
{
    assert( m_gamepad );
    while ( m_isRunning ) {
        std::list<Gamepad::Event> events = m_gamepad->pollChanges();
        // script might not be written yet
        if ( !m_script ) {
            continue;
        }
        LockGuard lockGuard( m_mutex );
        for ( const Gamepad::Event& it : events ) {
            if ( m_hasNativeEvent ) {
                m_script->call( "GGPAD_nativeEvent" ) << it._type << it._code << it._value;
            } else if ( m_hasEvent ) {
                m_script->call( "GGPAD_event" ) << it.button << it.value;
            }
        }
    }
}

std::string Binding::gamepadName() const
{
    return m_gamepadName + std::string( m_gamepad ? "" : " (disconnected)" );
}

bool Binding::stopIfNeeded()
{
    if ( !m_isRunning ) {
        return false;
    }
    if ( m_gamepad && m_gamepad->isConnected() ) {
        return false;
    }

    bool wasRunning = m_isRunning;
    m_isRunning = false;
    if ( wasRunning ) {
        m_eventThread.join();
        m_updateThread.join();
    }
    delete m_gamepad;
    m_gamepad = nullptr;
    return true;
}
