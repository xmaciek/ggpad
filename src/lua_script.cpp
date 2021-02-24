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

#include "log.hpp"

#include <lua.hpp>

#include <cassert>
#include <cstdlib>
#include <fstream>

namespace lua {

enum Address : int {
    eKey = -2
    , eValue = -1
};

Script::Script() noexcept
{
    m_vm = lua_newstate( &Allocator::luaRealloc, &m_allocator );
    luaL_openlibs( m_vm );
}

Script::~Script() noexcept
{
    lua_close( m_vm );
}

void Script::bindTable( std::string_view name, const Record* begin, const Record* end )
{
    assert( m_vm );
    lua_newtable( m_vm );
    for ( const Record* it = begin; it != end; ++it ) {
        lua_pushinteger( m_vm, it->value );
        lua_setfield( m_vm, Address::eKey, it->name.data() );
    }
    lua_setglobal( m_vm, name.data() );
    const int stackCount = lua_gettop( m_vm );
    assert( stackCount == 0 );
}

void Script::doFile( const std::filesystem::path& a_fileName )
{
    assert( m_vm );
    std::ifstream ifs( a_fileName, std::ios::binary | std::ios::ate );
    assert( ifs.is_open() );
    std::string text;
    text.resize( (std::size_t)ifs.tellg() + 1 );
    ifs.seekg( 0 );
    ifs.read( text.data(), text.size() );
    ifs.close();
    text.back() = 0;
    const int ec = luaL_dostring( m_vm, text.c_str() );
    if ( ec != LUA_OK ) {
        LOG( LOG_ERROR, "%s", lua_tostring( m_vm, Address::eValue ) );
    }
}

static Script::Variant getFromStack( Script::vm_type* vm, int idx )
{
    switch ( lua_type( vm, idx ) ) {
        case LUA_TNUMBER:
            return Script::Variant( (int64_t)lua_tonumber( vm, idx ) );
        case LUA_TSTRING:
            return Script::Variant( lua_tostring( vm, idx ) );
        default:
            return Script::Variant();
    }
}

std::vector<Script::Pair> Script::getTable( const char* name )
{
    std::vector<Script::Pair> v;
    lua_getglobal( m_vm, name );
    if ( !lua_istable( m_vm, Address::eValue ) ) {
        return v;
    }

    lua_pushnil( m_vm );
    while ( lua_next( m_vm, Address::eKey ) ) {
        v.emplace_back(
            getFromStack( m_vm, Address::eKey ),
            getFromStack( m_vm, Address::eValue )
        );
        pop();
    }
    return v;
}

void Script::registerFunction( const char* name, callback_type cb )
{
    lua_register( m_vm, name, cb );
}

int Script::stackCount( vm_type* vm )
{
    return lua_gettop( vm );
}

template<>
int Script::get<int>( vm_type* vm, std::size_t n )
{
    return lua_tonumber( vm, n );
}

template <>
bool Script::get<bool>( vm_type* vm, std::size_t n )
{
    return !!lua_tointeger( vm, n );
}

void Script::pop()
{
    lua_pop( m_vm, 1 );
}

bool Script::hasError() const
{
    return m_lastErrorCode != LUA_OK;
}

bool Script::getFunc( std::string_view funcName )
{
    assert( m_vm );
    assert( funcName.data() );
    lua_State* vm = reinterpret_cast<lua_State*>( m_vm );
    const int ret = lua_getglobal( vm, funcName.data() );
    return ret == LUA_TFUNCTION;
}

int Script::call( int num )
{
    lua_State* vm = reinterpret_cast<lua_State*>( m_vm );
    const int ret = lua_pcall( vm, num, 0, 0 );
    if ( ret != LUA_OK ) {
        LOG( LOG_ERROR, "%s\n", lua_tostring( vm, -1 ) );
    }
    assert( stackCount( m_vm )  == 0 );
    return ret;
}

void Script::push_arg( short v )
{
    lua_pushinteger( reinterpret_cast<lua_State*>( m_vm ), v );
}

void Script::push_arg( int v )
{
    lua_pushinteger( reinterpret_cast<lua_State*>( m_vm ), v );
}

void Script::push_arg( double v )
{
    lua_pushnumber( reinterpret_cast<lua_State*>( m_vm ), v );
}

} // namespace lua
