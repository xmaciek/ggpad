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

#include "allocator.hpp"

#include <algorithm>
#include <cassert>
#include <cstdlib>

#define PRINT_USAGE 0
#if PRINT_USAGE
#include <iostream>
#endif // PRINT_USAGE

Allocator::~Allocator() noexcept
{
    assert( m_totalUsage == 0 );
    assert( m_blockUsage.empty() );
}

Allocator::value_type* Allocator::allocate( Allocator::size_type size ) noexcept
{
    if ( size == 0 ) {
        return nullptr;
    }
    m_totalUsage += size;
    m_blockUsage.emplace( size, 0 ).first->second++;
    return reinterpret_cast<value_type*>( std::malloc( size ) );
}

void Allocator::deallocate( Allocator::value_type* ptr, Allocator::size_type size ) noexcept
{
    m_totalUsage -= size;
    const auto it = m_blockUsage.find( size );
    if ( it != m_blockUsage.cend() ) {
        it->second--;
        if ( it->second == 0 ) {
            m_blockUsage.erase( it );
        }
    }
    std::free( ptr );
}

void* Allocator::luaRealloc( void* context, void* ptr, Allocator::size_type oldSize, Allocator::size_type newSize ) noexcept
{
    Allocator* allocator = reinterpret_cast<Allocator*>( context );

#if PRINT_USAGE
    std::cout << "[ REALLOC ]\n";
    for ( const auto& it : allocator->usage() ) {
        std::cout << it.first << " : " << it.second << std::endl;
    }
    std::cout << "[ ======= ]\n";
#endif // PRINT_USAGE

    if ( !ptr ) {
        return allocator->allocate( newSize );
    }

    value_type* data = reinterpret_cast<value_type*>( ptr );
    if ( newSize == 0 ) {
        allocator->deallocate( data, oldSize );
        return nullptr;
    }

    value_type* newData = allocator->allocate( newSize );
    assert( sizeof( value_type ) == 1 ); // sanity check for ptr arithmetic
    std::copy( data, data + std::min( oldSize, newSize ), newData );
    allocator->deallocate( data, oldSize );
    return newData;
}

Allocator::difference_type Allocator::totalUsage() const
{
    return m_totalUsage;
}

std::vector<std::pair<size_t, size_t>> Allocator::usage() const
{
    std::vector<std::pair<size_t, size_t>> ret;
    ret.reserve( m_blockUsage.size() );
    std::copy( m_blockUsage.cbegin(), m_blockUsage.cend(), std::back_inserter( ret ) );
    return ret;
}
