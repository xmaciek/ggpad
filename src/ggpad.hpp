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

#include <list>
#include <memory>
#include <thread>
#include <utility>

#include "binding.hpp"
#include "comm.hpp"
#include "gui.hpp"
#include "gui_controller_model.hpp"
#include "macros.hpp"
#include "watcher.hpp"
#include "settings.hpp"
#include "systemevent.hpp"

class GGPAD {
    DISABLE_COPY( GGPAD );

public:
    using BindList = std::vector<Binding::Ptr>;

private:
    static GGPAD* s_instance;
    Settings m_settings;

    bool m_isRunning = true;

    BindList m_list;

    std::unique_ptr<Watcher> m_deviceWatcher;
    std::unique_ptr<SystemEvent> m_systemEvent;
    std::unique_ptr<Gui> m_gui;

    Comm* m_clientComm = nullptr;
    std::thread m_threadClientMessages;
    void processClientMessages();

    void runScript( uint64_t id, const std::filesystem::path& );
    void stopScript( uint64_t id );

    Binding* findById( uint64_t );

public:
    GGPAD( Comm* );
    ~GGPAD();

    static void setKeyboard( uint32_t, bool );
    static void mouseMove( uint32_t a_axis, int32_t a_delta );

    void quit();
    int exec();
};
