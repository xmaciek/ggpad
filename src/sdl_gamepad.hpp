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

#pragma once

#include "gamepad.hpp"

#include <mutex>

#include "idcounter.hpp"

#include <SDL2/SDL.h>

class SDLGamepad : public Gamepad {

    std::mutex m_bottleneck{};
    std::list<Event> m_list{};
    SDL_GameController* m_gamecontroller = nullptr;
    IdCounter m_uid{};
    RuntimeId m_runtimeId{};
    uint32_t m_vidpid = 0;
    bool m_isConnected = true;

public:
    ~SDLGamepad() noexcept;
    SDLGamepad( SDL_GameController* ) noexcept;
    virtual uint32_t vidpid() const override;
    virtual uint64_t uid() const override;
    virtual RuntimeId runtimeId() const override;
    virtual bool isConnected() const override;
    virtual std::string displayName() const override;
    virtual void disconnect() override;
};
