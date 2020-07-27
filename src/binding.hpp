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
#include "queue.hpp"
#include "script.hpp"

#include <atomic>
#include <filesystem>
#include <list>
#include <memory>
#include <mutex>
#include <thread>

class Binding {
    DISABLE_COPY( Binding );
    void* m_editor = nullptr;

    Queue<Gamepad::Event> m_queue;
    std::thread m_pollThread;

    Barrier m_scriptBarrier;
    std::mutex m_mutexScript;
    std::thread m_updateThread;
    std::thread m_eventThread;
    std::filesystem::path m_currentScriptFile;

    std::unique_ptr<Gamepad> m_gamepad;
    std::string m_gamepadName;

    bool m_lastGamepadConnectionState = false;

    void pollLoop();
    void eventLoop();
    void updateLoop();

public:
    using Ptr = std::unique_ptr<Binding>;
    using LockGuard = std::lock_guard<std::mutex>;

    void setScript( Script* );
    Script::Function m_updateFunc;
    Script::Function m_eventFunc;
    Script::Function m_nativeEventFunc;
    std::atomic_bool m_isRunning = false;
    std::atomic_bool m_isRunningScript = false;

    uint64_t m_gamepadId = 0;
    Script* m_script = nullptr;

    Binding() = default;
    ~Binding();

    void setGamepad( Gamepad* );
    void run();
    void stopScript();
    void stopPoll();
    void stop();

    std::string gamepadName() const;
    bool connectionStateChanged();

    void startScript();
    void startPoll();
    void discardEventQueue();

    void* editor() const;
    void setEditor( void* );

    void setCurrentScriptFile( const std::filesystem::path& );
};
