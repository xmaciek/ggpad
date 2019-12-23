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

#pragma once

#include <utility>

namespace lua {

struct vm_type;

class Function {
private:
    vm_type* m_vm = nullptr;
    const char* m_funcName = nullptr;

    bool getFunc() const;
    bool call( int );


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

public:
    ~Function() noexcept = default;
    Function() noexcept = default;
    Function( vm_type*, const char* ) noexcept;
    operator bool () const;

    template <class... ARGS>
    void operator () ( ARGS&&... args )
    {
        getFunc() && call( push( std::forward<ARGS>( args )... ) );
    }

};

} // namespace lua
