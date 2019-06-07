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

uint64_t IdCounter::create( uint32_t vidpid )
{
    std::lock_guard<std::mutex> lockGuard( m_mutex );
    uint32_t candidate = 0;
    for ( int i = 0; i < 100; i++ ) {
        candidate = (uint64_t)vidpid | ( (uint64_t)i << 32 );
        if ( !std::binary_search( m_counts.begin(), m_counts.end(), candidate ) ) {
            break;
        }
    }
    m_counts.push_back( candidate );
    std::sort( m_counts.begin(), m_counts.end() );
    return candidate;
}

void IdCounter::release( uint64_t id )
{
    std::lock_guard<std::mutex> lockGuard( m_mutex );
    std::remove( m_counts.begin(), m_counts.end(), id );
}

IdCounter g_idCounter;
