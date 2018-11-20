set( BINNAME "ggpad" )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )

find_package( Threads REQUIRED )
find_package( Lua REQUIRED )
find_package( PkgConfig REQUIRED )

pkg_check_modules( LIBUDEV libudev )
if( !LIBUDEV_FOUND )
    message( FATAL_ERROR "unable to locate libudev" )
endif()

set( SOURCES
    binding.cpp
    gamepad_linux_default.cpp
    gamepad_linux_xbox_one_s_bluetooth_0x045E02FD.cpp
    gamepad_linux.cpp
    ggpad.cpp
    lua_script.cpp
    lua_script_function.cpp
    main.cpp
    systemevent_linux.cpp
    watcher_udev.cpp
)

set( LIBRARIES
    ${CMAKE_THREAD_LIBS_INIT}
    ${LIBUDEV_LIBRARIES}
    ${LUA_LIBRARIES}
)

set( INCLUDES
    ${LUA_INCLUDE_DIR}
)

set( CMAKE_CXX_STANDARD 14 )
add_executable( ${BINNAME} ${SOURCES} )
target_link_libraries( ${BINNAME} ${LIBRARIES} )
target_include_directories( ${BINNAME} PUBLIC ${INCLUDES} )
