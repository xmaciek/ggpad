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

#include <linux/input.h>
#include "gamepad_linux.hpp"

constexpr static const MapTable Ddefault[] = {
    { EV_KEY, BTN_SOUTH, 0, 1, ConversionType::Digital, Gamepad::A, Gamepad::unknown, 0, 1 }
    , { EV_KEY, BTN_EAST, 0, 1, ConversionType::Digital, Gamepad::B, Gamepad::unknown, 0, 1 }
    , { EV_KEY, BTN_NORTH, 0, 1, ConversionType::Digital, Gamepad::Y, Gamepad::unknown, 0, 1 }
    , { EV_KEY, BTN_WEST, 0, 1, ConversionType::Digital, Gamepad::X, Gamepad::unknown, 0, 1 }
    , { EV_KEY, BTN_TL, 0, 1, ConversionType::Digital, Gamepad::L1, Gamepad::unknown, 0, 1 }
    , { EV_KEY, BTN_TR, 0, 1, ConversionType::Digital, Gamepad::R1, Gamepad::unknown, 0, 1 }
    , { EV_KEY, BTN_THUMBL, 0, 1, ConversionType::Digital, Gamepad::L3, Gamepad::unknown, 0, 1 }
    , { EV_KEY, BTN_THUMBR, 0, 1, ConversionType::Digital, Gamepad::R3, Gamepad::unknown, 0, 1 }
    , { EV_KEY, BTN_START, 0, 1, ConversionType::Digital, Gamepad::START, Gamepad::unknown, 0, 1 }
    , { EV_KEY, BTN_SELECT, 0, 1, ConversionType::Digital, Gamepad::SELECT, Gamepad::unknown, 0, 1 }
    , { EV_ABS, ABS_X, 0, 0xFFFF, ConversionType::Analog, Gamepad::LX, Gamepad::unknown, -127, 127 }
    , { EV_ABS, ABS_Y, 0, 0xFFFF, ConversionType::Analog, Gamepad::LY, Gamepad::unknown, 127, -127 }
    , { EV_ABS, ABS_RX, 0, 0xFFFF, ConversionType::Analog, Gamepad::RX, Gamepad::unknown, -127, 127 }
    , { EV_ABS, ABS_RY, 0, 0xFFFF, ConversionType::Analog, Gamepad::RY, Gamepad::unknown, 127, -127 }
    , { EV_ABS, ABS_Z, 0, 1023, ConversionType::Analog, Gamepad::L2, Gamepad::unknown, 0, 127 }
    , { EV_ABS, ABS_RZ, 0, 1023, ConversionType::Analog, Gamepad::R2, Gamepad::unknown, 0, 127 }
    , { EV_ABS, ABS_HAT0X, -1, 1, ConversionType::Digital, Gamepad::LEFT, Gamepad::RIGHT, 0, 1 }
    , { EV_ABS, ABS_HAT0Y, -1, 1, ConversionType::Digital, Gamepad::UP, Gamepad::DOWN, 0, 1 }
    , { 0 }
};

const MapTable* GamepadDefault = Ddefault;
