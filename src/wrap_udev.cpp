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

#include "wrap_udev.hpp"

#include <cassert>

UDevDevice::UDevDevice( udev_device* p )
: m_ptr( p )
{
}

UDevDevice::~UDevDevice()
{
    if ( m_ptr ) {
        udev_device_unref( m_ptr );
        m_ptr = 0;
    }
}

UDevDevice::UDevDevice( UDevDevice&& other )
: m_ptr( other.m_ptr )
{
    other.m_ptr = 0;
}

UDevDevice& UDevDevice::operator = ( UDevDevice&& other )
{
    m_ptr = other.m_ptr;
    other.m_ptr = 0;
    return *this;
}

UDevDevice::operator udev_device* () const
{
    return m_ptr;
}

UDevDevice::operator bool () const
{
    return m_ptr;
}

bool UDevDevice::operator ! () const
{
    return !m_ptr;
}

const char* UDevDevice::action() const
{
    assert( m_ptr );
    return udev_device_get_action( m_ptr );
}

const char* UDevDevice::devnode() const
{
    assert( m_ptr );
    return udev_device_get_devnode( m_ptr );
}

const char* UDevDevice::syspath() const
{
    assert( m_ptr );
    return udev_device_get_syspath( m_ptr );
}

const char* UDevDevice::propertyValue( const char* name ) const
{
    assert( m_ptr );
    assert( name );
    return udev_device_get_property_value( m_ptr, name );
}
