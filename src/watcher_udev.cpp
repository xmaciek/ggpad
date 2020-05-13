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

#include "watcher_udev.hpp"

#include <algorithm>
#include <cstring>
#include <memory>

#include <poll.h>

#include "gamepad_linux.hpp"
#include "log.hpp"
#include "wrap_udev.hpp"

namespace {

// waits up to 10ms for event
bool waitForEvent( int fd )
{
    ::pollfd p{};
    p.fd = fd;
    p.events = POLLIN;
    return ::poll( &p, 1, 10 ) > 0;
}

} // namespace

WatcherUDev::WatcherUDev()
{
    m_udevPtr = udev_new();
    if ( !m_udevPtr ) {
        return;
    }

    m_udevMonitorPtr = udev_monitor_new_from_netlink( m_udevPtr, "udev" );
    if ( !m_udevMonitorPtr ) {
        return;
    }

    int errnum = udev_monitor_filter_add_match_subsystem_devtype( m_udevMonitorPtr, "input", nullptr );
    if ( errnum != 0 ) {
        return;
    }

    errnum = udev_monitor_enable_receiving( m_udevMonitorPtr );
    if ( errnum != 0 ) {
        return;
    }

    m_udevEnumeratePtr = udev_enumerate_new( m_udevPtr );
    if ( !m_udevEnumeratePtr ) {
        return;
    }

    errnum = udev_enumerate_add_match_subsystem( m_udevEnumeratePtr, "input" );
}

WatcherUDev::~WatcherUDev()
{
    if ( m_udevEnumeratePtr ) {
        udev_enumerate_unref( m_udevEnumeratePtr );
    }

    if ( m_udevMonitorPtr ) {
        udev_monitor_unref( m_udevMonitorPtr );
    }

    if ( m_udevPtr ) {
        udev_unref( m_udevPtr );
    }
}

std::list<Gamepad*> WatcherUDev::currentDevices()
{
    std::list<Gamepad*> list;
    int errnum = udev_enumerate_scan_devices( m_udevEnumeratePtr );

    udev_list_entry* devListEntry = nullptr;
    udev_list_entry* allDevices = udev_enumerate_get_list_entry( m_udevEnumeratePtr );

    for ( devListEntry = allDevices; devListEntry; devListEntry = udev_list_entry_get_next( devListEntry ) ) {
        const char* path = udev_list_entry_get_name( devListEntry );
        if ( !path ) {
            continue;
        }

        UDevDevice device = udev_device_new_from_syspath( m_udevPtr, path );
        const char* devPath = device.devnode();
        if ( !devPath ) {
            continue;
        }

        const char* isJoystick = device.propertyValue( "ID_INPUT_JOYSTICK" );
        if ( !isJoystick || *isJoystick == '0' ) {
            continue;
        }

        if ( std::strstr( path, "virtual" ) ) {
            continue;
        }

        if ( !std::strstr( path, "event" ) ) {
            continue;
        }

        list.push_back( new GamepadLinux( devPath ) );
    }

    return list;
}

std::list<Gamepad*> WatcherUDev::newDevices()
{
    std::list<Gamepad*> list;
    while ( waitForEvent( udev_monitor_get_fd( m_udevMonitorPtr ) ) ) {
        UDevDevice device = udev_monitor_receive_device( m_udevMonitorPtr );
        if ( !device ) {
            continue;
        }

        const char* isJoystick = device.propertyValue( "ID_INPUT_JOYSTICK" );
        if ( !isJoystick || *isJoystick == '0' ) {
            continue;
        }

        const char* path = device.syspath();
        if ( !path ) {
            continue;
        }

        if ( std::strstr( path, "virtual" ) ) {
            continue;
        }

        if ( !std::strstr( path, "event" ) ) {
            continue;
        }

        constexpr const char* ACTIONS[] = { "add", "online" };
        const char* action = device.action();
        bool isNew = false;
        for ( const char* it : ACTIONS ) {
            if ( std::strcmp( action, it ) == 0 ) {
                isNew = true;
                break;
            }
        }
        if ( !isNew ) {
            continue;
        }

        LOG( LOG_DEBUG, "New device detected as %s", action );
        const char* devPath = device.devnode();
        if ( !devPath ) {
            continue;
        }

        list.push_back( new GamepadLinux( devPath ) );
    }
    return list;
}
