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

using namespace std::chrono_literals;

Binding::~Binding()
{
    stop();
    delete m_script;
}

void Binding::run()
{
    startScript();
    startPoll();
}

void Binding::updateLoop()
{
    assert( m_script );
    const double deltaTime = 5.0 / 1000;
    while ( m_isRunningScript.load() ) {
        std::this_thread::sleep_for( 5ms );
        LockGuard lockGuard( m_mutexScript );
        m_script->call( "GGPAD_update", deltaTime );
    }
}

void Binding::pushEvent( const Gamepad::Event& event )
{
    if ( !m_isRunningScript.load() ) {
        return;
    }
    m_queue.emplace( event );
    m_scriptBottleneck.notify();
}

void Binding::eventLoop()
{
    std::optional<Gamepad::Event> ev;
    while ( m_isRunningScript.load() ) {
        m_scriptBottleneck.wait_for( 5ms );

        while ( m_isRunningScript.load() && ( ev = m_queue.pop() ) ) {
            const Gamepad::Event& it = *ev;
            {
                LockGuard lg( m_mutexScript );
                m_script && m_script->call( "GGPAD_event", (int)it.button, it.value );
            }
        }
    }
}


void Binding::setGamepad( Gamepad* gamepad )
{
    assert( gamepad );
    assert( !m_gamepad );
    assert( !m_isRunning.load() );
    m_gamepad.reset( gamepad );
    m_gamepadId = m_gamepad->uid();
    m_gamepadName = m_gamepad->displayName();

}

std::string Binding::gamepadName() const
{
    // checking for ->isConnected() is kind of pointless,
    // you either have gamepad object which is implicitly always connected or you don't.
    const bool isConnected = m_gamepad && m_gamepad->isConnected();
    const std::string status = isConnected ? std::string() : std::string( " (disconnected)" );
    return m_gamepadName + status;
}

std::string Binding::scriptStatusAsText() const
{
    return m_isRunningScript.load() ? "Running" : "Not Running";
}

bool Binding::connectionStateChanged()
{
    const bool currentConnectionState = connectionState();
    if ( currentConnectionState == m_lastGamepadConnectionState ) {
        return false;
    }
    m_lastGamepadConnectionState = currentConnectionState;
    // if gamepad disconnection is detected, we should get rid of it
    if ( !currentConnectionState ) {
        stopPoll();
        m_gamepad.reset();
    }
    return true;
}

bool Binding::scriptStateChanged()
{
    const bool oldState = m_lastScriptState;
    m_lastScriptState = m_isRunningScript.load();
    return oldState != m_lastScriptState;
}

void Binding::stop()
{
    stopPoll();
    stopScript();
}

void Binding::stopScript()
{
    std::lock_guard<std::mutex> lg( m_threadOperation );
    m_isRunningScript.store( false );
    if ( m_eventThread && m_eventThread->joinable() ) {
        m_eventThread->join();
    }
    m_eventThread.reset();

    if ( m_updateThread && m_updateThread->joinable() ) {
        m_updateThread->join();
    }
    m_updateThread.reset();
}

void Binding::stopPoll()
{
    m_isRunning.store( false );
}

void Binding::startScript()
{
    std::lock_guard<std::mutex> lg( m_threadOperation );
    bool expected = false;
    if ( !m_isRunningScript.compare_exchange_weak( expected, true ) ) {
        return;
    }
    assert( !m_eventThread );
    assert( !m_updateThread );
    m_eventThread.emplace( &Binding::eventLoop, this );
    m_updateThread.emplace( &Binding::updateLoop, this );
}

void Binding::startPoll()
{
    m_isRunning.store( true );
}

void Binding::setScript( Script* sc )
{
    assert( sc );
    assert( !m_isRunningScript.load() );
    delete m_script;
    m_script = sc;
}

void* Binding::editor() const
{
    return m_editor;
}

void Binding::setEditor( void* e )
{
    m_editor = e;
}

void Binding::setCurrentScriptFile( const std::filesystem::path& p )
{
    m_currentScriptFile = p;
}

void Binding::discardEventQueue()
{
    m_queue.clear();
}

bool Binding::connectionState() const
{
    return m_gamepad && m_gamepad->isConnected();
}

std::filesystem::path Binding::currentScriptFile() const
{
    return m_currentScriptFile;
}

void Binding::disconnect()
{
    if ( m_gamepad ) {
        m_gamepad->disconnect();
    }
}

uint64_t Binding::gamepadId() const
{
    return m_gamepad ? m_gamepad->uid() : 0xfefefefe;
}

Gamepad::RuntimeId Binding::gamepadRuntimeId() const
{
    return m_gamepad ? m_gamepad->runtimeId() : Gamepad::RuntimeId{};
}
