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

#include "idcounter.hpp"

#include <algorithm>
#include <mutex>
#include <set>

static std::mutex& mutex()
{
    static std::mutex mtx;
    return mtx;
}

static std::set<uint64_t>& set()
{
    static std::set<uint64_t> s;
    return s;
}

IdCounter::IdCounter( uint32_t vidpid )
{
    std::lock_guard<std::mutex> lockGuard( mutex() );
    uint32_t candidate = 0;
    for ( int i = 0; i < 100; i++ ) {
        candidate = (uint64_t)vidpid | ( (uint64_t)i << 32 );
        if ( set().count( candidate ) == 0 ) {
            break;
        }
    }
    set().insert( candidate );
    m_value = candidate;
}

IdCounter::~IdCounter()
{
    std::lock_guard<std::mutex> lockGuard( mutex() );
    set().erase( m_value );
}

IdCounter::operator uint64_t () const
{
    return m_value;
}
