# GGPAD Copyright 2019 Maciej Latocha
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

set( binname "ggpad" )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )

include( "${CMAKE_SOURCE_DIR}/platforms.cmake" )

find_package( Threads REQUIRED )
find_package( Qt5Widgets REQUIRED )
include( "${CMAKE_SOURCE_DIR}/lua.cmake" )
include( "${CMAKE_SOURCE_DIR}/libudev.cmake" )

set( link_libraries
    Threads::Threads
    Lua::Lua
    Qt5::Widgets
)

set( sources
    allocator.cpp
    barrier.cpp
    binding.cpp
    comm.cpp
    ggpad.cpp
    gui.cpp
    gui_controller_model.cpp
    gui_syntax_highlight.cpp
    idcounter.cpp
    lua_script.cpp
    lua_function.cpp
    log.cpp
    logview.cpp
    main.cpp
    settings.cpp
)

if ( GGPAD_LINUX )
    list( APPEND sources
        gamepad_linux_default.cpp
        gamepad_linux_xbox_one_s_bluetooth_0x045E02FD.cpp
        gamepad_linux_dualshock4_0x054C09CC.cpp
        gamepad_linux.cpp
        systemevent_linux.cpp
        wrap_udev.cpp
        watcher_udev.cpp
    )
    list( APPEND link_libraries
        Udev::Udev
        "stdc++fs"
    )
endif()

set( qheaders
    gui.hpp
    gui_controller_model.hpp
    gui_syntax_highlight.hpp
    logview.hpp
)

qt5_wrap_cpp( mocables ${qheaders} )


set( CMAKE_CXX_STANDARD 17 )
add_executable( ${binname} ${sources} ${mocables} )
target_link_libraries( ${binname} ${link_libraries} )
