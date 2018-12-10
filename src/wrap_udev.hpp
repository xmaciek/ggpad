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

#include <libudev.h>

#include "macros.hpp"

class UDevDevice {
    DISABLE_COPY( UDevDevice )

private:
    udev_device* m_ptr;

public:
    UDevDevice( udev_device* p = 0 );
    ~UDevDevice();

    UDevDevice( UDevDevice&& );
    UDevDevice& operator = ( UDevDevice&& );

    operator udev_device* () const;
    operator bool () const;
    bool operator ! () const;

    const char* action() const;
    const char* devnode() const;
    const char* syspath() const;
    const char* propertyValue( const char* name ) const;
};
