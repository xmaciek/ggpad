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

#include <linux/input.h>
#include <linux/uinput.h>

#include "macros.hpp"
#include "systemevent.hpp"

class SystemEventLinux : public SystemEvent {
    DISABLE_COPY( SystemEventLinux )

private:
    int32_t m_uinput = 0;
    ::input_event m_flush{};

    bool isValid() const;
    void sendEvent( uint32_t a_type, uint32_t a_code, int32_t a_value );

public:
    SystemEventLinux();
    virtual ~SystemEventLinux();

    virtual void keyboard( uint32_t a_key, bool a_state ) override;
    virtual void mouseMove( int32_t a_deltaX, int32_t a_deltaY ) override;

};
