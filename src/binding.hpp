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

#include "bottleneck.hpp"
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
    std::optional<std::thread> m_updateThread;
    std::optional<std::thread> m_eventThread;
    std::mutex m_threadOperation;
    std::mutex m_mutexScript;
    Bottleneck m_scriptBottleneck;

    std::filesystem::path m_currentScriptFile;

    std::unique_ptr<Gamepad> m_gamepad;
    std::string m_gamepadName;

    bool m_lastGamepadConnectionState = false;
    bool m_lastScriptState = false;

    void eventLoop();
    void updateLoop();

public:
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

    bool connectionStateChanged();
    bool connectionState() const;
    bool scriptStateChanged();
    std::string gamepadName() const;
    std::string scriptStatusAsText() const;
    uint64_t gamepadId() const;
    Gamepad::RuntimeId gamepadRuntimeId() const;
    void disconnect();

    void startScript();
    void startPoll();
    void discardEventQueue();

    void pushEvent( const Gamepad::Event& );

    void* editor() const;
    void setEditor( void* );

    void setCurrentScriptFile( const std::filesystem::path& );
    std::filesystem::path currentScriptFile() const;
};
