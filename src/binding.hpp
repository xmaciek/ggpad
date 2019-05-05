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

#include "gamepad.hpp"
#include "script.hpp"
#include "macros.hpp"

#include <memory>
#include <mutex>
#include <thread>

class Binding {
    DISABLE_COPY( Binding );

public:
    using Ptr = std::unique_ptr<Binding>;
    using LockGuard = std::lock_guard<std::mutex>;

    bool m_hasUpdate : 1;
    bool m_hasEvent : 1;
    bool m_hasNativeEvent : 1;
    bool m_isRunning : 1;

    std::string m_gamepadName;
    uint64_t m_gamepadId;
    Gamepad* m_gamepad;
    Script* m_script;

    std::thread m_updateThread;
    std::thread m_eventThread;
    std::mutex m_mutex;

    Binding();
    ~Binding();

    void run();
    void eventLoop();
    void updateLoop();

    std::string gamepadName() const;
    bool stopIfNeeded();
};
