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

typedef struct [[gnu::packed]] {
    std::uint16_t type;
    std::uint16_t code;
    std::int32_t minRange;
    std::int32_t maxRange;
    ConversionType conversionType;
    Gamepad::Button buttonMin;
    Gamepad::Button buttonMax;
    Gamepad::value_type minVal;
    Gamepad::value_type maxVal;
} MapTable;
