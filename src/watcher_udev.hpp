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

#include <utility>
#include <vector>

#include <libudev.h>

#include "watcher.hpp"

class WatcherUDev : public Watcher {
    DISABLE_COPY( WatcherUDev )

private:
    struct udev* m_udevPtr;
    struct udev_monitor* m_udevMonitorPtr;
    struct udev_enumerate* m_udevEnumeratePtr;

    // I'm to lazy to setup threaded udev socket notifier
    // when new devices are connected, for now at least.
    std::hash<const char*> m_hash;
    std::vector<std::size_t> m_knownDevices;
public:
    WatcherUDev();
    virtual ~WatcherUDev();

    virtual std::list<Gamepad*> newDevices() override;
};
