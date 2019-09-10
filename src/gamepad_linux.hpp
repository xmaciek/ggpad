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

#include "gamepad.hpp"
#include "idcounter.hpp"
#include "macros.hpp"
#include "maptable_linux.hpp"

#include <cstdint>

class GamepadLinux : public Gamepad {
    DISABLE_COPY( GamepadLinux )

public:
    using state_type = value_type[ Gamepad::max ];

private:
    int m_fd = -1;
    uint32_t m_vidpid = 0;
    IdCounter m_uid;
    state_type m_state = { 0 };
    TableInfo m_tableInfo;
    std::string m_displayName;

public:
    GamepadLinux( const char* a_devPath );
    virtual ~GamepadLinux();

    virtual uint32_t vidpid() const override;
    virtual uint64_t uid() const override;

    virtual bool isConnected() const override;
    virtual std::list<Gamepad::Event> pollChanges() override;
    virtual std::string displayName() const override;
};
