set( BINNAME "ggpad" )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )

find_package( Threads REQUIRED )
find_package( PkgConfig REQUIRED )
find_package( Qt5Widgets REQUIRED )
include( "${CMAKE_SOURCE_DIR}/lua.cmake" )

pkg_check_modules( LIBUDEV libudev )
if( !LIBUDEV_FOUND )
    message( FATAL_ERROR "unable to locate libudev" )
endif()

set( SOURCES
    barrier.cpp
    binding.cpp
    config.cpp
    gamepad_linux_default.cpp
    gamepad_linux_xbox_one_s_bluetooth_0x045E02FD.cpp
    gamepad_linux.cpp
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
    systemevent_linux.cpp
    wrap_udev.cpp
    watcher_udev.cpp
)

set( QHEADERS
    gui.hpp
    gui_controller_model.hpp
    gui_syntax_highlight.hpp
    logview.hpp
)

qt5_wrap_cpp( MOCABLES ${QHEADERS} )

set( LIBRARIES
    Threads::Threads
    ${LIBUDEV_LIBRARIES}
    Lua::Lua
    Qt5::Widgets
    "stdc++fs"
)

set( CMAKE_CXX_STANDARD 17 )
add_executable( ${BINNAME} ${SOURCES} ${MOCABLES} )
target_link_libraries( ${BINNAME} ${LIBRARIES} )
