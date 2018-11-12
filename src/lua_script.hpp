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

#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "macros.hpp"

struct lua_State;

class LuaScript {
    DISABLE_COPY( LuaScript )

private:
    std::mutex m_mutex;
    std::unique_ptr<struct lua_State, void(*)(struct lua_State*)> m_vm;

public:
    class Function {
        DISABLE_COPY( Function )

    private:
        friend LuaScript;
        struct lua_State* m_vm;
        int m_argc;
        Function( struct lua_State* a_vm, const char* a_funcName );

    public:
        Function( Function&& );
        Function& operator = ( Function&& );
        ~Function();
#define ARGUMENT( A_TYPE ) Function& operator << ( A_TYPE );
        ARGUMENT( int )
        ARGUMENT( double )
#undef ARGUMENT
    };

    typedef struct {
        const char* name;
        int value;
    } Record;

    typedef std::lock_guard<LuaScript> LockGuard;

    LuaScript();

    void lock();
    void unlock();

    struct lua_State* vm();
    void doFile( const char* a_fileName );
    void bindTable( const char* a_name, const std::vector<Record>& a_records );

    Function call( const char* a_funcName );

};



