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
#include <fstream>

#include "lua_script.hpp"
#include "lua.hpp" // see CMake Lua convention

#include "log.hpp"

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
    std::ifstream ifs( a_fileName, std::ios::binary | std::ios::ate );
    assert( ifs.is_open() );
    m_text.resize( (std::size_t)ifs.tellg() + 1 );
    ifs.seekg( 0 );
    ifs.read( m_text.data(), m_text.size() );
    ifs.close();
    m_text.back() = 0;
    const bool ret = luaL_dostring( m_vm.get(), m_text.c_str() ) == LUA_OK;
    if ( !ret ) {
        LOG( LOG_ERROR, "%s\n", lua_tostring( m_vm.get(), -1 ) );
    }
}

const std::string& LuaScript::text() const
{
    return m_text;
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
    return LuaScript::Function( reinterpret_cast<lua::vm_type*>( m_vm.get() ), a_funcName );
}

static LuaScript::Variant getFromStack( LuaScript::vm_type* vm, int idx )
{
    switch ( lua_type( vm, idx ) ) {
        case LUA_TNUMBER:
            return LuaScript::Variant( lua_tonumber( vm, idx ) );
        case LUA_TSTRING:
            return LuaScript::Variant( lua_tostring( vm, idx ) );
        default:
            return LuaScript::Variant();
    }
}

std::vector<LuaScript::Pair> LuaScript::getTable( const char* name )
{
    std::vector<LuaScript::Pair> v;
    lua_getglobal( m_vm.get(), name );
    if ( !lua_istable( m_vm.get(), -1 ) ) {
        return v;
    }

    lua_pushnil( m_vm.get() );
    while ( lua_next( m_vm.get(), -2 ) ) {
        v.push_back( { getFromStack( m_vm.get(), -2 ), getFromStack( m_vm.get(), -1 ) } );
        lua_pop( m_vm.get(), 1 );
    }
    return v;
}

void LuaScript::registerFunction( const char* name, callback_type cb )
{
    lua_register( m_vm.get(), name, cb );
}

int LuaScript::stackCount( vm_type* vm )
{
    return lua_gettop( vm );
}

template<>
int LuaScript::get<int>( vm_type* vm, std::size_t n )
{
    return lua_tointeger( vm, n );
}

template <>
bool LuaScript::get<bool>( vm_type* vm, std::size_t n )
{
    return !!lua_tointeger( vm, n );
}
