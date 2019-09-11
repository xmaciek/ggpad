// GGPAD Copyright 2019 Maciej Latocha
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

#include "lua_function.hpp"
#include "log.hpp"

#include "lua.hpp"

#include <cassert>

namespace lua {

Function::Function( vm_type* vm, const char* funcName )
: m_vm( vm )
, m_funcName( funcName )
{
}

Function::operator bool () const
{
    return m_vm && m_funcName && getFunc();
}

bool Function::getFunc() const
{
    assert( m_vm );
    assert( m_funcName );
    return lua_getglobal( reinterpret_cast<lua_State*>( m_vm ), m_funcName ) == LUA_TFUNCTION;
}

bool Function::call( int num )
{
    assert( m_vm );
    lua_State* vm = reinterpret_cast<lua_State*>( m_vm );
    const bool ret = lua_pcall( vm, num, 0, 0 ) == LUA_OK;
    if ( !ret ) {
        LOG( LOG_ERROR, "%s\n", lua_tostring( vm, -1 ) );
    }
    return ret;
}

void Function::push_arg( short v )
{
    lua_pushinteger( reinterpret_cast<lua_State*>( m_vm ), v );
}

void Function::push_arg( int v )
{
    lua_pushinteger( reinterpret_cast<lua_State*>( m_vm ), v );
}

void Function::push_arg( double v )
{
    lua_pushnumber( reinterpret_cast<lua_State*>( m_vm ), v );
}



} // namespace lua
