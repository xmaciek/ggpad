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

#include "systemevent_linux.hpp"

#include <cassert>
#include <cerrno>
#include <cstring>

#include <fcntl.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <unistd.h>

#include "log.hpp"
#include "systemevent_linux_table.hpp"


namespace {

bool enableInputCodes( int32_t uinput )
{
    if ( ::ioctl( uinput, UI_SET_EVBIT, EV_KEY ) < 0 ) {
        return false;
    }
    if ( ::ioctl( uinput, UI_SET_EVBIT, EV_REL ) < 0 ) {
        return false;
    }

    for ( uint16_t it : g_keyButtonLUT ) {
        ::ioctl( uinput, UI_SET_KEYBIT, it );
    }
    ::ioctl( uinput, UI_SET_RELBIT, REL_X );
    ::ioctl( uinput, UI_SET_RELBIT, REL_Y );
    return true;
}

bool createDevice( int32_t uinput, const uinput_user_dev* device )
{
    assert( device );
    const ssize_t bytes = ::write( uinput, device, sizeof( uinput_user_dev ) );
    if ( bytes != sizeof( uinput_user_dev ) ) {
        return false;
    }

    return ::ioctl( uinput, UI_DEV_CREATE ) == 0;
}

void flush( int32_t uinput )
{
    ::input_event ev{};
    ev.type = EV_SYN;
    ev.code = SYN_REPORT;
    ::gettimeofday( &ev.time, 0 );
    ::write( uinput, &ev, sizeof( ev ) );
}

} // namespace

SystemEventLinux::SystemEventLinux()
{
    LOG( LOG_DEBUG, "Initializing uinput... " );
    errno = 0;
    m_uinput = ::open( "/dev/uinput", O_WRONLY | O_NONBLOCK );
    int e = errno;
    if ( !isValid() ) {
        LOG( LOG_ERROR, "<b><font color=\"red\">/dev/uinput %s</font></b>", ::strerror( e ) );
        return;
    } else {
        LOG( LOG_DEBUG, "OK" );
    }

    LOG( LOG_DEBUG, "Enabling input codes... " );
    if ( !enableInputCodes( m_uinput ) ) {
        e = errno;
        LOG( LOG_ERROR, "Fail: %s", ::strerror( e ) );
        return;
    } else {
        LOG( LOG_DEBUG, "OK" );
    }

    ::uinput_user_dev virtualDevice{};
    std::strcpy( virtualDevice.name, "GGPAD-virtual-input-device" );
    virtualDevice.id.bustype = BUS_USB;
    virtualDevice.id.vendor  = 0x1234;
    virtualDevice.id.product = 0xfedc;
    virtualDevice.id.version = 1;
    LOG( LOG_DEBUG, "Creating virtual device... " );
    if ( !createDevice( m_uinput, &virtualDevice ) ) {
        e = errno;
        LOG( LOG_ERROR, "<font color=\"red\">Fail: %s</font>", ::strerror( e ) );
        return;
    } else {
        LOG( LOG_DEBUG, "OK" );
    }

}

SystemEventLinux::~SystemEventLinux()
{
    if ( !isValid() ) {
        return;
    }
    ::ioctl( m_uinput, UI_DEV_DESTROY );
    ::close( m_uinput );
}

void SystemEventLinux::sendEvent( uint32_t type, uint32_t code, int32_t value )
{
    if ( !isValid() ) {
        LOG( LOG_ERROR, "<font color=\"red\">Unable to send system event</font>" )
        return;
    }
    input_event event{};
    event.type = type;
    event.code = code;
    event.value = value;
    ::gettimeofday( &event.time, 0 );
    const int written = ::write( m_uinput, &event, sizeof( input_event ) );
    assert( written == sizeof( input_event ) );
    flush( m_uinput );
}


void SystemEventLinux::keyboard( uint32_t key, bool state )
{
    const size_t index = std::min<size_t>( key, std::size( g_keyButtonLUT ) );
    if ( index == std::size( g_keyButtonLUT ) ) {
        LOG( LOG_ERROR, "Key code %u not supported", key );
        return;
    }
    sendEvent( EV_KEY, g_keyButtonLUT[ index ], state );
}

void SystemEventLinux::mouseMove( int32_t a_deltaX, int32_t a_deltaY )
{
    if ( a_deltaX != 0 ) {
        sendEvent( EV_REL, REL_X, a_deltaX );
    }
    if ( a_deltaY != 0 ) {
        sendEvent( EV_REL, REL_Y, a_deltaY );
    }
}

// I'd like not to use QCursor, but I can't get this to work with uinput
#include <QCursor>
void SystemEventLinux::mouseSet( int32_t x, int32_t y )
{
    QCursor::setPos( x, y );
}

bool SystemEventLinux::isValid() const
{
    return m_uinput > 0;
}
