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

#include "nocopy.hpp"

#include <cstdint>
#include <map>
#include <type_traits>
#include <vector>

class Allocator : nocopy {
public:
    using value_type = uint8_t;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using propagate_on_container_move_assignment = std::true_type;
    using is_always_equal = std::true_type;

private:
    difference_type m_totalUsage = 0;
    std::map<size_t,size_t> m_blockUsage;

public:
    ~Allocator() noexcept;
    Allocator() noexcept = default;
    value_type* allocate( size_type ) noexcept;
    void deallocate( value_type*, size_type ) noexcept;

    difference_type totalUsage() const;
    std::vector<std::pair<size_t, size_t>> usage() const;

    static void* luaRealloc( void* context, void* ptr, size_type oldSize, size_type newSize ) noexcept;

};
