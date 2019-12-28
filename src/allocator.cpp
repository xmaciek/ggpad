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

#include <cassert>
#include <cstdlib>

Allocator::~Allocator() noexcept
{
    assert( m_totalUsage == 0 );
}

Allocator::value_type* Allocator::allocate( Allocator::size_type size ) noexcept
{
    m_totalUsage += size;
    return reinterpret_cast<value_type*>( std::malloc( size ) );
}

void Allocator::deallocate( Allocator::value_type* ptr, Allocator::size_type size ) noexcept
{
    m_totalUsage -= size;
    std::free( ptr );
}

void* Allocator::luaRealloc( void* context, void* ptr, Allocator::size_type oldSize, Allocator::size_type newSize ) noexcept
{
    Allocator* allocator = reinterpret_cast<Allocator*>( context );
    value_type* data = reinterpret_cast<value_type*>( ptr );
    if ( newSize == 0 ) {
        allocator->deallocate( data, oldSize );
        return nullptr;
    }

    if ( !ptr ) {
        return allocator->allocate( newSize );
    }

    allocator->m_totalUsage -= oldSize;
    allocator->m_totalUsage += newSize;
    return std::realloc( ptr, newSize );
}

Allocator::difference_type Allocator::totalUsage() const
{
    return m_totalUsage;
}
