// GGPAD Copyright 2020 Maciej Latocha
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

#include <list>
#include <mutex>
#include <optional>

template <typename T>
class Queue {
    DISABLE_COPY( Queue );

    std::mutex m_mutex;
    std::list<T> m_list{};

public:
    using value_type = T;
    Queue() = default;
    ~Queue() = default;

    template <class... ARGS>
    void emplace( ARGS&&... args )
    {
        std::lock_guard<std::mutex> lock( m_mutex );
        m_list.emplace_back( std::forward<ARGS>( args )... );
    }

    std::optional<value_type> pop()
    {
        std::lock_guard<std::mutex> lock( m_mutex );
        if ( m_list.empty() ) return {};
        std::optional<value_type> tmp = m_list.front();
        m_list.pop_front();
        return tmp;
    }
};
