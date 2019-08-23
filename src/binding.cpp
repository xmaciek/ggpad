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

constexpr std::chrono::high_resolution_clock::duration operator "" _ms( unsigned long long int i )
{
    return std::chrono::milliseconds( i );
}

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
    stop();
    delete m_gamepad;
    delete m_script;
}

void Binding::run()
{
    LockGuard lockGuard( m_mutex );
    if ( m_isRunning ) {
        return;
    }
    m_isRunning = true;
    m_pollThread = std::thread( &Binding::pollLoop, this );
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
        std::this_thread::sleep_for( 5_ms );
        LockGuard lockGuard( m_mutexScript );
        m_script->call( "GGPAD_update" )( deltaTime );
    }
}

void Binding::pollLoop()
{
    assert( m_gamepad );
    while ( m_isRunning ) {
        std::list<Gamepad::Event> events = m_gamepad->pollChanges();
        if ( events.empty() ) {
            continue;
        }
        LockGuard lg( m_mutex );
        m_eventQueue.splice( m_eventQueue.end(), std::move( events ) );
        m_scriptBarrier.notify();
    }

}

void Binding::eventLoop()
{
    while ( m_isRunning ) {
        m_scriptBarrier.wait_for( 5_ms );
        std::list<Gamepad::Event> events;
        {
            LockGuard lockGuard( m_mutex );
            std::swap( events, m_eventQueue );
        }
        if ( events.empty() ) {
            continue;
        }

        LockGuard lg( m_mutexScript );
        for ( const Gamepad::Event& it : events ) {
            if ( m_hasNativeEvent ) {
                m_script->call( "GGPAD_nativeEvent" )( it._type, it._code, it._value );
            } else if ( m_hasEvent ) {
                m_script->call( "GGPAD_event" )( (int)it.button, it.value );
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

    stop();

    delete m_gamepad;
    m_gamepad = nullptr;
    return true;
}

void Binding::stop()
{
    m_isRunning = false;

    if ( m_eventThread.joinable() ) {
        m_eventThread.join();
    }
    if ( m_updateThread.joinable() ) {
        m_updateThread.join();
    }
    if ( m_pollThread.joinable() ) {
        m_pollThread.join();
    }
    LockGuard lg( m_mutexScript );
    m_eventQueue.clear();
}
