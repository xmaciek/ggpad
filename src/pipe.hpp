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

#include "macros.hpp"

#include <fcntl.h>
#include <unistd.h>

#include <memory>
#include <optional>
#include <utility>

template <typename T>
class Pipe {
    DISABLE_COPY( Pipe );

    enum Channel : int {
        eRead = 0
        , eWrite = 1
        , eMAX = 2
    };
    int m_pipe[ Channel::eMAX ]{};

public:
    using value_type = T;

    Pipe()
    {
        ::pipe2( m_pipe, O_NONBLOCK );
    }

    ~Pipe()
    {
        while ( pop() ) {}
        ::close( m_pipe[ Channel::eWrite ] );
        ::close( m_pipe[ Channel::eRead ] );
    }

    template <class... ARGS>
    void emplace( ARGS&&... args )
    {
        alignas( alignof( value_type ) ) uint8_t buffer[ sizeof( value_type ) ];
        new ( buffer ) value_type( std::forward<ARGS>( args )... );

        const bool ok = ::write( m_pipe[ Channel::eWrite ], buffer, sizeof( value_type ) ) < 0;
        if ( !ok ) {
            std::destroy_at<value_type>( reinterpret_cast<value_type*>( buffer ) );
        }
    }

    std::optional<value_type> pop()
    {
        alignas( alignof( value_type ) ) uint8_t buffer[ sizeof( value_type ) ];
        const bool ok = ::read( m_pipe[ Channel::eRead ], buffer, sizeof( value_type ) ) < 0;
        return ok
            ? std::move( *reinterpret_cast<value_type*>( buffer ) )
            : std::optional<value_type>();
    }
};
