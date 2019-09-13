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

#include "barrier.hpp"
#include "gamepad.hpp"
#include "macros.hpp"
#include "pipe.hpp"
#include "script.hpp"

#include <condition_variable>
#include <list>
#include <memory>
#include <mutex>
#include <thread>

class Binding {
    DISABLE_COPY( Binding );
    void* m_editor = nullptr;

    Pipe<Gamepad::Event> m_queue;
    std::thread m_pollThread;

    Barrier m_scriptBarrier;
    std::mutex m_mutexScript;
    std::thread m_updateThread;
    std::thread m_eventThread;

public:
    using Ptr = std::unique_ptr<Binding>;
    using LockGuard = std::lock_guard<std::mutex>;

    void setScript( Script* );
    Script::Function m_updateFunc;
    Script::Function m_eventFunc;
    Script::Function m_nativeEventFunc;
    bool m_isRunning = false;
    bool m_isRunningScript = false;

    std::string m_gamepadName;
    uint64_t m_gamepadId = 0;
    Gamepad* m_gamepad = nullptr;
    Script* m_script = nullptr;

    Binding() = default;
    ~Binding();

    void run();
    void pollLoop();
    void eventLoop();
    void updateLoop();

    std::string gamepadName() const;
    bool stopIfNeeded();
    void stop();

    void* editor() const;
    void setEditor( void* );
};
