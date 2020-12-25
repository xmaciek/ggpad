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

#include "nocopy.hpp"

#include <cstdint>

class SystemEventLinux : nocopy {
    int32_t m_uinput = 0;

    bool isValid() const;
    void sendEvent( uint32_t a_type, uint32_t a_code, int32_t a_value );

public:
    SystemEventLinux();
    ~SystemEventLinux();

    void keyboard( uint32_t a_key, bool a_state );
    void mouseMove( int32_t a_deltaX, int32_t a_deltaY );
    void mouseSet( int32_t x, int32_t y );

};
