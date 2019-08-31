find_package( Lua REQUIRED )
if ( NOT TARGET Lua::Lua )
    add_library( Lua::Lua UNKNOWN IMPORTED )
    set_target_properties( Lua::Lua PROPERTIES
        IMPORTED_LOCATION  "${LUA_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${LUA_INCLUDE_DIR}"
    )
endif()
