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

#pragma once

#include <cstdint>

#include "gamepad.hpp"
#include "macros.hpp"
#include "maptable_linux.hpp"

class GamepadLinux : public Gamepad {
    DISABLE_COPY( GamepadLinux )

public:
    typedef value_type state_type[ Gamepad::max ];

private:
    int m_fd;
    uint32_t m_vidpid;
    state_type m_state;
    const MapTable* m_mapTable;

public:
    GamepadLinux( const char* a_devPath );
    virtual ~GamepadLinux();

    virtual bool isConnected() const override;
    virtual std::list<Gamepad::Event> pollChanges() override;
};
