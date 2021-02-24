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

#include "allocator.hpp"
#include "nocopy.hpp"

#include <filesystem>
#include <memory>
#include <string>
#include <variant>
#include <vector>

struct lua_State;

namespace lua {

class Script : nocopy {

public:
    using vm_type = lua_State;
    using callback_type = int(*)( vm_type* );

private:
    Allocator m_allocator;
    vm_type* m_vm = nullptr;

    int m_lastErrorCode = 0;
    static int stackCount( vm_type* );

    template <typename T>
    static T get( vm_type*, std::size_t );

    void pop();

    int errorCode() const;
    bool getFunc( std::string_view );

    template <typename T>
    void push_arg( T );

    void push_arg( short );
    void push_arg( int );
    void push_arg( double );

    template <typename T1>
    int push( T1&& t1 )
    {
        push_arg( t1 );
        return 1;
    }
    template <typename T1, typename T2>
    int push( T1&& t1, T2&& t2 )
    {
        push_arg( t1 );
        push_arg( t2 );
        return 2;
    }
    template <typename T1, typename T2, typename T3>
    int push( T1&& t1, T2&& t2, T3&& t3 )
    {
        push_arg( t1 );
        push_arg( t2 );
        push_arg( t3 );
        return 3;
    }

    int call( int );

public:
    using Variant = std::variant<std::string,int64_t>;
    using Pair = std::pair<Variant,Variant>;

    struct Record {
        int value;
        std::string_view name;

        constexpr Record( int v, std::string_view n ) noexcept
        : value{ v }
        , name{ n }
        {}
    };

    ~Script() noexcept;
    Script() noexcept;

    void doFile( const std::filesystem::path& );

    void bindTable( std::string_view name, const Record* begin, const Record* end );
    std::vector<Pair> getTable( const char* a_name );

    // TODO: this should return std::error_code
    template <class... ARGS>
    int call( std::string_view funcName, ARGS&&... args )
    {
        return getFunc( funcName )
            ? call( push( std::forward<ARGS>( args )... ) )
            : -1;
    }

    void registerFunction( const char*, callback_type );

    template <typename FUNC_TYPE, FUNC_TYPE FUNC, typename ARG0, typename ARG1>
    static int facade( vm_type* vm )
    {
        if ( Script::stackCount( vm ) != 2 ) {
            return 0;
        }
        (*FUNC)(
            Script::get<ARG0>( vm, 1 ),
            Script::get<ARG1>( vm, 2 )
        );
        return 1;
    }

    bool hasError() const;
};

} // namespace lua
