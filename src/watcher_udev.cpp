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
#include <memory>
#include <cstring>

WatcherUDev::WatcherUDev()
: Watcher()
, m_udevPtr( nullptr )
, m_udevMonitorPtr( nullptr )
, m_udevEnumeratePtr( nullptr )
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

std::list<Gamepad*> WatcherUDev::newDevices()
{
    std::list<Gamepad*> list;
    int errnum = udev_enumerate_scan_devices( m_udevEnumeratePtr );

    struct udev_list_entry* devListEntry = 0;
    struct udev_list_entry* allDevices = udev_enumerate_get_list_entry( m_udevEnumeratePtr );

    for ( devListEntry = allDevices; devListEntry; devListEntry = udev_list_entry_get_next( devListEntry ) ) {
        const char* path = udev_list_entry_get_name( devListEntry );
        if ( !path ) {
            continue;
        }

        const std::size_t pathHash = m_hash( path );
        if ( std::binary_search( m_knownDevices.cbegin(), m_knownDevices.cend(), pathHash ) ) {
            continue;
        }

        struct udev_device* device = udev_device_new_from_syspath( m_udevPtr, path );
        const char* devPath = udev_device_get_devnode( device );
        if ( !devPath ) {
            udev_device_unref( device );
            continue;
        }

        const char* isJoystick = udev_device_get_property_value( device, "ID_INPUT_JOYSTICK" );
        if ( !isJoystick || *isJoystick == '0' ) {
            udev_device_unref( device );
            continue;
        }
        if ( std::strstr( path, "virtual" ) ) {
            udev_device_unref( device );
            continue;
        }
        if ( !std::strstr( path, "event" ) ) {
            udev_device_unref( device );
            continue;
        }

        m_knownDevices.push_back( pathHash );
        std::sort( m_knownDevices.begin(), m_knownDevices.end() );

        fprintf( stdout, "path: %s\n", devPath );
    }

    return list;
}
