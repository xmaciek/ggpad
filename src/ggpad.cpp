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

#include "ggpad.hpp"
#include "gamepad.hpp"
#include "watcher_udev.hpp"

#include <thread>
#include <chrono>

static const std::vector<LuaScript::Record> GAMEPAD_TABLE {
    { "unknown", 0 }
#define MAKE_ENUM( NAME ) , { #NAME , Gamepad::NAME }
#include "button_enum.def"
#undef MAKE_ENUM
};

GGPAD::GGPAD()
{
    m_deviceWatcher = std::make_unique<WatcherUDev>();
}

int GGPAD::exec()
{
    std::list<Gamepad*> list = m_deviceWatcher->newDevices();
    if ( list.empty() ) {
        return 0;
    }

    LuaScript script;
    script.bindTable( "Gamepad", GAMEPAD_TABLE );
    script.doFile( "test1.lua" );
    while ( true ) {
        std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
        std::list<Gamepad::Event> events;
        Gamepad::Event e;
        while ( list.front()->pollChanges( &e ) ) {
            events.push_back( e );
        }
        for ( const Gamepad::Event& it : events ) {
            script.call( "GGPAD_buttonChanged" ) << it.button << it.value;
        }
    }
    return 0;
}
