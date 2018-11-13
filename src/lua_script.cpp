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

#include <cassert>
#include <cstdlib>

#include "lua_script.hpp"
#include "lua.hpp" // see CMake Lua convention

static void* l_alloc( void*, void* ptr, std::size_t, std::size_t nsize )
{
    if ( !nsize ) {
        std::free( ptr );
        return 0;
    }
    return std::realloc( ptr, nsize );
}

LuaScript::LuaScript()
: m_vm( lua_newstate( l_alloc, 0 ), lua_close )
{
    luaL_openlibs( m_vm.get() );
}

void LuaScript::lock()
{
    m_mutex.lock();
}

void LuaScript::unlock()
{
    m_mutex.unlock();
}

struct lua_State* LuaScript::vm()
{
    return m_vm.get();
}

void LuaScript::bindTable( const char* a_name, const std::vector<LuaScript::Record>& a_table )
{
    assert( m_vm );
    lua_newtable( m_vm.get() );
    for ( const LuaScript::Record& it : a_table ) {
        lua_pushinteger( m_vm.get(), it.value );
        lua_setfield( m_vm.get(), -2, it.name );
    }
    lua_setglobal( m_vm.get(), a_name );
}

void LuaScript::doFile( const char* a_fileName )
{
    assert( m_vm );
    luaL_dofile( m_vm.get(), a_fileName );
}

bool LuaScript::hasFunction( const char* a_funcName )
{
    assert( a_funcName );
    lua_getglobal( m_vm.get(), a_funcName );
    const bool ret = lua_isfunction( m_vm.get(), -1 );
    lua_pop( m_vm.get(), 1 );
    return ret;
}

LuaScript::Function LuaScript::call( const char* a_funcName )
{
    return LuaScript::Function( m_vm.get(), a_funcName );
}

