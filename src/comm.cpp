// GGPAD Copyright 2020 Maciej Latocha ( latocha.maciek@gmail.com )
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

#include "comm.hpp"

void Comm::pushToServer( Message&& msg )
{
    m_queueA.emplace( std::forward<Message>( msg ) );
}

void Comm::pushToClient( Message&& msg )
{
    m_queueB.emplace( std::forward<Message>( msg ) );
}

std::optional<Message> Comm::popFromClient()
{
    return m_queueA.pop();
}

std::optional<Message> Comm::popFromServer()
{
    return m_queueB.pop();
}


void Comm::notifyClient()
{
    m_barrierB.notify();
}

void Comm::notifyServer()
{
    m_barrierA.notify();
}

void Comm::waitForClient( const std::chrono::high_resolution_clock::duration& d )
{
    m_barrierA.wait_for( d );
}

void Comm::waitForServer( const std::chrono::high_resolution_clock::duration& d )
{
    m_barrierB.wait_for( d );
}
