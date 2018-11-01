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

#include "gamepad_linux.hpp"

#include <cassert>
#include <cstring>
#include <cstdio>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>

extern const MapTable* xboxOneSBluetooth_0x045E02FD;

static const MapTable* driverFixForVidPid( uint32_t vidpid )
{
    switch ( vidpid ) {
        case 0x045E02FD: return xboxOneSBluetooth_0x045E02FD;
        default: return 0;
    }
}

GamepadLinux::GamepadLinux( const char* a_devPath )
: Gamepad()
, m_fd( -1 )
, m_vidpid( 0 )
{
    assert( a_devPath );
    m_fd = ::open( a_devPath, O_RDWR | O_NONBLOCK );
    if ( m_fd < 0 ) {
        assert( !"unable to open device path" );
        return;
    }

    struct input_id id;
    std::memset( &id, 0, sizeof( id ) );
    ioctl( m_fd, EVIOCGID, &id );
    m_vidpid = id.vendor;
    m_vidpid <<= 16;
    m_vidpid |= id.product;
    fprintf( stdout, "%p : %08X\n", this, m_vidpid );
    m_mapTable = driverFixForVidPid( m_vidpid );
}

GamepadLinux::~GamepadLinux()
{
    if ( m_fd >= 0 ) {
        ::close( m_fd );
    }
}

bool GamepadLinux::pollChanges( Gamepad::Event* a_event )
{
    if ( !m_mapTable ) {
        return false;
    }

    struct input_event ev;
    const int ret = ::read( m_fd, &ev, sizeof( ev ) );
    int e = errno;
    if ( ret != sizeof( ev ) ) {
        return false;
    }
    if ( e != EAGAIN ) {
        return false;
    }

    const MapTable* entry = m_mapTable;
    while ( entry->type ) {
        if ( entry->type == ev.type && entry->code == ev.code ) {
            break;
        }
        entry++;
    }
    if ( !entry->type ) {
        return false;
    }

    if ( entry->conversionType == ConversionType::Digital && entry->type == EV_KEY ) {
        a_event->button = entry->buttonMin;
        a_event->value = ev.value;
        return true;
    }

    if ( entry->conversionType == ConversionType::Analog && entry->type == EV_ABS ) {
        int v = entry->maxVal - entry->minVal;
        double d = (double)ev.value / entry->maxRange;
        a_event->value = v * d + entry->minVal;
        a_event->button = entry->buttonMin;
        return true;
    }

    if ( entry->conversionType == ConversionType::Digital && entry->type == EV_ABS ) {
        if ( ev.value <= entry->minRange ) {
            a_event->button = entry->buttonMin;
            a_event->value = m_state[ entry->buttonMin ] = entry->maxVal;
        } else if ( ev.value >= entry->maxRange ) {
            a_event->button = entry->buttonMax;
            a_event->value = m_state[ entry->buttonMax ] = entry->maxVal;
        } else {
            a_event->button = m_state[ entry->buttonMin ] ? entry->buttonMin : entry->buttonMax;
            a_event->value = m_state[ a_event->button ] = entry->minVal;
        }
        return true;
    }
    return false;
}
