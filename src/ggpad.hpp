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

#include "gamepad.hpp"
#include "lua_script.hpp"
#include "macros.hpp"
#include "watcher.hpp"
#include "systemevent.hpp"



class GGPAD {
    DISABLE_COPY( GGPAD );

private:
    static GGPAD* s_instance;

    bool m_isRunning;
    std::thread m_threadEvents;
    std::thread m_threadUpdate;

    typedef std::pair<Gamepad*,LuaScript*> Binding;
    std::list<Binding> m_list;

    std::unique_ptr<Watcher> m_deviceWatcher;
    std::unique_ptr<SystemEvent> m_systemEvent;

    using KbdFunc = void( uint32_t, bool );
    static void setKeyboardState( uint32_t, bool );

    using MouseFunc = void( uint32_t, int32_t );
    static void mouseMove( uint32_t a_axis, int32_t a_delta );


    void eventsLoop();
    void updateLoop();

public:
    GGPAD();
    ~GGPAD();

    int exec();
};
