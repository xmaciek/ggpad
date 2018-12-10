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

#include <list>

#include "gamepad.hpp"
#include "macros.hpp"

class Watcher {
    DISABLE_COPY( Watcher )

public:
    Watcher() = default;
    virtual ~Watcher() = default;

    virtual std::list<Gamepad*> currentDevices() = 0;
    virtual std::list<Gamepad*> newDevices() = 0;
};
