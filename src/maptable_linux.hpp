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

#include <cstdint>
#include "gamepad.hpp"

enum class ConversionType : std::int8_t {
    Analog
    , Digital
};

struct [[gnu::packed]] MapTable {
    std::uint16_t type;
    std::uint16_t code;
    std::int32_t minRange;
    std::int32_t maxRange;
    ConversionType conversionType;
    Gamepad::Button buttonMin;
    Gamepad::Button buttonMax;
    Gamepad::value_type minVal;
    Gamepad::value_type maxVal;
    constexpr static int spaceship( const MapTable& lhs, const MapTable& rhs )
    {
        if ( lhs.type < rhs.type ) {
            return -1;
        } else if ( rhs.type < lhs.type ) {
            return 1;
        } else if ( lhs.code < rhs.code ) {
            return -1;
        } else if ( rhs.code < lhs.code ) {
            return 1;
        } else {
            return 0;
        }
    }
};

typedef struct [[gnu::packed]] {
    const MapTable* ptr;
    std::size_t size;
} TableInfo;

template<std::size_t S>
class MapSort {
private:
    MapTable m_table[ S ];

public:
    constexpr MapSort( const MapTable table[ S ] )
    : m_table{}
    {
        for ( std::size_t i = 0; i < S; i++ ) {
            m_table[ i ] = table[ i ];
        }
        for ( std::size_t i = 0; i < S; i++ )
        for ( std::size_t j = 0; j < S; j++ ) {
            if ( MapTable::spaceship( m_table[ i ], m_table[ j ] ) < 0 ) {
                MapTable tmp = std::move( m_table[ i ] );
                m_table[ i ] = std::move( m_table[ j ] );
                m_table[ j ] = std::move( tmp );
            }
        }
    }
    constexpr operator TableInfo () const
    {
        return TableInfo{ m_table, S };
    }
};
