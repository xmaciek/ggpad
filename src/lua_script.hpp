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
#include <string>
#include <variant>
#include <vector>

#include "macros.hpp"
#include "lua_function.hpp"

struct lua_State;

namespace lua {

class Script {
    DISABLE_COPY( Script )

public:
    using vm_type = struct lua_State;
    using callback_type = int(*)(vm_type*);

private:
    std::unique_ptr<vm_type, void(*)(vm_type*)> m_vm;
    std::string m_text;

    static int stackCount( vm_type* );

    template <typename T>
    static T get( vm_type*, std::size_t );

    void pop();

public:
    using Function = lua::Function;

    using Variant = std::variant<std::string,int64_t>;
    using Pair = std::pair<Variant,Variant>;

    typedef struct {
        const char* name;
        int value;
    } Record;

    Script();

    void doFile( const char* a_fileName );
    const std::string& text() const;

    void bindTable( const char* a_name, const std::vector<Record>& a_records );
    std::vector<Pair> getTable( const char* a_name );

    bool hasFunction( const char* a_funcName );
    Function call( const char* a_funcName );

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
};

} // namespace lua
