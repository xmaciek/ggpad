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

#include "sdl_gamepad.hpp"

#include <cassert>
#include <array>

#include "log.hpp"

SDLGamepad::~SDLGamepad() noexcept
{
    assert( m_gamecontroller );
    SDL_GameControllerClose( m_gamecontroller );
}

SDLGamepad::SDLGamepad( SDL_GameController* pad ) noexcept
: m_gamecontroller{ pad }
{
    assert( pad );
    m_vidpid = SDL_GameControllerGetVendor( pad );
    m_vidpid <<= 16;
    m_vidpid |= SDL_GameControllerGetProduct( pad );
    m_uid = m_vidpid;
}

std::string SDLGamepad::displayName() const
{
    assert( m_gamecontroller );
    const char* name = SDL_GameControllerName( m_gamecontroller );
    return name ? name : "Unnamed";
}

uint32_t SDLGamepad::vidpid() const
{
    return m_vidpid;
}

uint64_t SDLGamepad::uid() const
{
    return m_uid;
}

bool SDLGamepad::isConnected() const
{
    return m_isConnected;
}

void SDLGamepad::disconnect()
{
    m_isConnected = false;
}

std::list<Gamepad::Event> SDLGamepad::pollChanges()
{
    std::list<Event> ret{};
    std::lock_guard<std::mutex> lg{ m_bottleneck };
    std::swap( ret, m_list );
    return ret;
}

void SDLGamepad::push( Gamepad::Button button, int16_t v )
{
    std::lock_guard<std::mutex> lg{ m_bottleneck };
    m_list.emplace_back( button, v );
}
