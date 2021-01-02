// GGPAD Copyright 2021 Maciej Latocha ( latocha.maciek@gmail.com )
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

#include <cstdint>
#include <variant>

template <typename TVariant, typename TMember, std::size_t TIndex = 0>
constexpr uint64_t indexOf() noexcept
{
    static_assert( TIndex < std::variant_size_v<TVariant> );
    if constexpr ( !std::is_same_v<TMember, std::variant_alternative_t<TIndex, TVariant>> ) {
        return indexOf<TVariant, TMember, TIndex + 1>();
    }
    return TIndex;
}
