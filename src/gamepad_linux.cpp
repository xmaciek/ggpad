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
#include <cstdlib>
#include <cstring>

#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <linux/input.h>

#include "idcounter.hpp"
#include "log.hpp"

extern const TableInfo GamepadDefault;
extern const TableInfo xboxOneSBluetooth_0x045E02FD;

static TableInfo driverFixForVidPid( uint32_t vidpid )
{
    switch ( vidpid ) {
        case 0x045E02FD: return xboxOneSBluetooth_0x045E02FD;
        default: return GamepadDefault;
    }
}

GamepadLinux::GamepadLinux( const char* a_devPath )
: Gamepad()
, m_fd( -1 )
, m_vidpid( 0 )
{
    LOG( LOG_DEBUG, "opening device %s\n", a_devPath );
    assert( a_devPath );
    m_fd = ::open( a_devPath, O_RDONLY | O_NONBLOCK );
    if ( m_fd < 0 ) {
        LOG( LOG_ERROR, "unable to open device path\n" );
        assert( !"unable to open device path" );
        return;
    }

    struct input_id id;
    std::memset( &id, 0, sizeof( id ) );
    ioctl( m_fd, EVIOCGID, &id );
    m_vidpid = id.vendor;
    m_vidpid <<= 16;
    m_vidpid |= id.product;
    m_uid = g_idCounter.create( m_vidpid );
    LOG( LOG_DEBUG, "Found device : %08X : %016X\n", this, m_vidpid, m_uid );
    m_tableInfo = driverFixForVidPid( m_vidpid );
}

GamepadLinux::~GamepadLinux()
{
    LOG( LOG_DEBUG, "Disconnected device %p : %016X\n", this, m_vidpid );
    if ( m_fd >= 0 ) {
        ::close( m_fd );
    }
    g_idCounter.release( m_uid );
}

uint32_t GamepadLinux::vidpid() const
{
    return m_vidpid;
}

uint64_t GamepadLinux::uid() const
{
    return m_uid;
}

static bool waitForEvent( int* fd )
{
    assert( fd );
    assert( *fd > 0 );

    struct pollfd p;
    p.fd = *fd;
    p.events = POLLIN;
    const int ret = poll( &p, 1, 40 );

    // device lost
    if ( ret < 0 ) {
        ::close( *fd );
        *fd = -1;
        return false;
    }

    // OK or timeout
    return ret > 0;
}

static bool getEvent( int* fd, struct input_event* ev )
{
    assert( fd );
    assert( *fd > 0 );
    assert( ev );

    const int ret = ::read( *fd, ev, sizeof( struct input_event ) );
    const int e = errno;
    switch ( e ) {
        default:
            LOG( LOG_ERROR, "Unhandled errno %d\n", e );
            [[fallthrough]];

        case ENODEV: // device lost
            ::close( *fd );
            *fd = -1;
            return false;

        case 0:
        case EAGAIN:
            return ret == sizeof( struct input_event );;
    }
}

template<typename T>
int bcmp( const void* lhs, const void* rhs )
{
    return T::spaceship( *reinterpret_cast<const T*>( lhs ), *reinterpret_cast<const T*>( rhs ) );
}

static void convertEvent( const TableInfo& tableInfo, const struct input_event* a_evIn,
                          Gamepad::Event* a_evOut, GamepadLinux::state_type& a_state )
{
    assert( a_evIn );
    assert( a_evOut );

    a_evOut->_type = a_evIn->type;
    a_evOut->_code = a_evIn->code;
    a_evOut->_value = a_evIn->value;

    const MapTable key{ a_evIn->type, a_evIn->code };
    const MapTable* entry = (const MapTable*)std::bsearch( &key, tableInfo.ptr, tableInfo.size, sizeof( MapTable ), &bcmp<MapTable> );
    if ( !entry ) {
        return;
    }

    if ( entry->conversionType == ConversionType::Digital && entry->type == EV_KEY ) {
        a_evOut->button = entry->buttonMin;
        a_evOut->value = a_evIn->value;
        return;
    }

    if ( entry->conversionType == ConversionType::Analog && entry->type == EV_ABS ) {
        int v = entry->maxVal - entry->minVal;
        double d = (double)a_evIn->value / entry->maxRange;
        a_evOut->value = v * d + entry->minVal;
        a_evOut->button = entry->buttonMin;
        return;
    }

    if ( entry->conversionType == ConversionType::Digital && entry->type == EV_ABS ) {
        if ( a_evIn->value <= entry->minRange ) {
            a_evOut->button = entry->buttonMin;
            a_evOut->value = a_state[ entry->buttonMin ] = entry->maxVal;
        } else if ( a_evIn->value >= entry->maxRange ) {
            a_evOut->button = entry->buttonMax;
            a_evOut->value = a_state[ entry->buttonMax ] = entry->maxVal;
        } else {
            a_evOut->button = a_state[ entry->buttonMin ] ? entry->buttonMin : entry->buttonMax;
            a_evOut->value = a_state[ a_evOut->button ] = entry->minVal;
        }
    }
}

static bool isBlacklistEvent( const struct input_event& ev )
{
    switch ( ev.type ) {
        case 0:
        case EV_MSC:
            return true;
        default:
            return false;
    }
}

std::list<Gamepad::Event> GamepadLinux::pollChanges()
{
    if ( !m_tableInfo.ptr ) {
        return {};
    }

    if ( m_fd <= 0 ) {
        return {};
    }

    if ( !waitForEvent( &m_fd ) ) {
        return {};
    }

    // grab all events before doing conversion to avoid read-convert-read-convert endless loop
    struct input_event ev{ 0 };
    std::list<struct input_event> events;
    while ( getEvent( &m_fd, &ev ) ) {
        if ( !isBlacklistEvent( ev ) ) {
            events.push_back( ev );
        }
    }

    std::list<Gamepad::Event> list;
    for ( const struct input_event& it : events ) {
        list.push_back( Gamepad::Event() );
        convertEvent( m_tableInfo, &it, &list.back(), m_state );
    };
    return list;
}

bool GamepadLinux::isConnected() const
{
    return m_fd > 0;
}

