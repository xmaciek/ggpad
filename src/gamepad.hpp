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
#include "macros.hpp"
#include "lua_script.hpp"

class Gamepad {
    DISABLE_COPY( Gamepad )

public:
    enum Button : int {
        unknown
#define MAKE_ENUM( E ) , E
#include "button_enum.def"
#undef MAKE_ENUM
    };

    typedef std::int8_t value_type;

    typedef struct {
        Button button;
        value_type value;
    } Event;

    Gamepad() = default;
    virtual ~Gamepad() = default;

    virtual bool pollChanges( Event* ) = 0;
};
