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

#include <cassert>
#include <cstring>
#include <iostream>
#include <mutex>

#include <fcntl.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <unistd.h>

#include "systemevent_linux.hpp"

static bool enableInputCodes( int32_t uinput )
{
    if ( ::ioctl( uinput, UI_SET_EVBIT, EV_KEY ) < 0 ) {
        return false;
    }
    if ( ::ioctl( uinput, UI_SET_EVBIT, EV_REL ) < 0 ) {
        return false;
    }

    #define MAKE_ENUM( A, B ) ::ioctl( uinput, UI_SET_KEYBIT, KEY_ ## B );
    #include "key_enum.def"
    #undef MAKE_ENUM

    return true;
}

static bool createDevice( int32_t uinput, const struct uinput_user_dev* device )
{
    assert( device );
    const ssize_t bytes = ::write( uinput, device, sizeof( struct uinput_user_dev ) );
    if ( bytes != sizeof( struct uinput_user_dev ) ) {
        return false;
    }

    return ::ioctl( uinput, UI_DEV_CREATE ) == 0;
}

SystemEventLinux::SystemEventLinux()
: SystemEvent()
, m_uinput( 0 )
{
    std::cout << "Initializing uinput... " << std::flush;
    m_uinput = ::open( "/dev/uinput", O_WRONLY | O_NONBLOCK );
    uint64_t e = errno;
    if ( m_uinput < 1 ) {
        std::cout << "/dev/uintput : " << ::strerror( e ) << std::endl;
        return;
    } else {
        std::cout << "OK" << std::endl;
    }

    std::cout << "Enabling input codes... " << std::flush;
    if ( !enableInputCodes( m_uinput ) ) {
        e = errno;
        std::cout << "Fail: " << ::strerror( e ) << std::endl;
        return;
    } else {
        std::cout << "OK" << std::endl;
    }

    ::memset( &m_virtualDevice, 0, sizeof( struct uinput_user_dev ) );
    ::strcpy( m_virtualDevice.name, "GGPAD-virtual-input-device" );
    m_virtualDevice.id.bustype = BUS_USB;
    m_virtualDevice.id.vendor  = 0x1234;
    m_virtualDevice.id.product = 0xfedc;
    m_virtualDevice.id.version = 1;
    std::cout << "Creating virtual device... " << std::flush;
    if ( !createDevice( m_uinput, &m_virtualDevice ) ) {
        e = errno;
        std::cout << "Fail: " << ::strerror( e ) << std::endl;
        return;
    } else {
        std::cout << "OK" << std::endl;
    }

    ::memset( &m_flush, 0, sizeof( struct input_event ) );
    m_flush.type = EV_SYN;
    m_flush.code = SYN_REPORT;
    m_flush.value = 0;
}

SystemEventLinux::~SystemEventLinux()
{
    ::ioctl( m_uinput, UI_DEV_DESTROY );
    ::close( m_uinput );
}

void SystemEventLinux::sendEvent( uint64_t type, uint64_t code, uint64_t value )
{
    struct input_event event;
    ::memset( &event, 0, sizeof( struct input_event ) );
    event.type = type;
    event.code = code;
    event.value = value;
    ::gettimeofday( &event.time, 0 );
    const auto written = ::write( m_uinput, &event, sizeof( struct input_event ) );
    assert( written == sizeof( struct input_event ) );

    ::gettimeofday( &m_flush.time, 0 );
    ::write( m_uinput, &m_flush, sizeof( struct input_event ) );
}

void SystemEventLinux::keyboard( uint64_t key, bool state )
{
    sendEvent( EV_KEY, key, state );
}
