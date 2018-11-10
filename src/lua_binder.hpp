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

#include <cassert>

#include "lua.hpp" // see CMake Lua convention

class LuaBinder {
public:
    template <typename T>
    static T get( struct lua_State*, std::size_t );

    template <typename FUNC_TYPE, FUNC_TYPE FUNC, typename ARG0, typename ARG1>
    static int facade( struct lua_State* a_vm )
    {
        if ( lua_gettop( a_vm ) != 2 ) {
            return 0;
        }
        (*FUNC)(
            LuaBinder::get<ARG0>( a_vm, 1 ),
            LuaBinder::get<ARG1>( a_vm, 2 )
        );
        return 1;
    }
};


template <>
int LuaBinder::get<int>( struct lua_State* a_vm, std::size_t a_n )
{
    return lua_tointeger( a_vm, a_n );
}

template <>
bool LuaBinder::get<bool>( struct lua_State* a_vm, std::size_t a_n )
{
    return !!lua_tointeger( a_vm, a_n );
}
