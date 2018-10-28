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

#include "lua_script.hpp"
#include "lua.hpp" // see CMake convention for Lua

#include <cassert>

LuaScript::Function::Function( lua_State* a_vm, const char* a_funcName )
: m_vm( a_vm )
, m_argc( 0 )
{
    lua_getglobal( m_vm, a_funcName );
}

LuaScript::Function::Function( LuaScript::Function&& f )
: m_vm( f.m_vm )
, m_argc( f.m_argc )
{
    f.m_vm = 0;
    f.m_argc = 0;
}

LuaScript::Function& LuaScript::Function::operator = ( LuaScript::Function&& f )
{
    m_vm = f.m_vm;
    f.m_vm = 0;

    m_argc = f.m_argc;
    f.m_argc = 0;

    return *this;
}

LuaScript::Function::~Function()
{
    if ( m_vm ) {
        lua_pcall( m_vm, m_argc, 0, 0 );
    }
}

LuaScript::Function& LuaScript::Function::operator << ( int a )
{
    assert( m_vm );
    lua_pushinteger( m_vm, a );
    m_argc++;
    return *this;
}

LuaScript::Function& LuaScript::Function::operator << ( double a )
{
    assert( m_vm );
    lua_pushnumber( m_vm, a );
    m_argc++;
    return *this;
}
