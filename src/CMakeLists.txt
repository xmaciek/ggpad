set( BINNAME "ggpad" )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} )

find_package( Threads REQUIRED )
find_package( Lua REQUIRED )

set( SOURCES
    main.cpp
    lua_script.cpp
    lua_script_function.cpp
)

set( LIBRARIES
    ${CMAKE_THREAD_LIBS_INIT}
    ${LUA_LIBRARIES}
)

set( INCLUDES
    ${LUA_INCLUDE_DIR}
)

set( CMAKE_CXX_STANDARD 14 )
add_executable( ${BINNAME} ${SOURCES} )
target_link_libraries( ${BINNAME} ${LIBRARIES} )
target_include_directories( ${BINNAME} PUBLIC ${INCLUDES} )
