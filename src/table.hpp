// GGPAD Copyright 2020 Maciej Latocha ( latocha.maciek@gmail.com )
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

#include <string_view>

#include "actuator.hpp"
#include "keyboard.hpp"
#include "script.hpp"

using namespace std::literals::string_view_literals;

static constexpr Script::Record g_tableGamepad[] = {
    { (int)Actuator::UP, "UP"sv },
    { (int)Actuator::DOWN, "DOWN"sv },
    { (int)Actuator::LEFT, "LEFT"sv },
    { (int)Actuator::RIGHT, "RIGHT"sv },
    { (int)Actuator::A, "A"sv },
    { (int)Actuator::B, "B"sv },
    { (int)Actuator::X, "X"sv },
    { (int)Actuator::Y, "Y"sv },
    { (int)Actuator::L1, "L1"sv },
    { (int)Actuator::L2, "L2"sv },
    { (int)Actuator::L3, "L3"sv },
    { (int)Actuator::LX, "LX"sv },
    { (int)Actuator::LY, "LY"sv },
    { (int)Actuator::R1, "R1"sv },
    { (int)Actuator::R2, "R2"sv },
    { (int)Actuator::R3, "R3"sv },
    { (int)Actuator::RX, "RX"sv },
    { (int)Actuator::RY, "RY"sv },
    { (int)Actuator::START, "START"sv },
    { (int)Actuator::SELECT, "SELECT"sv },
    { (int)Actuator::GUIDE, "GUIDE"sv },
};

static constexpr Script::Record g_tableMouse[] = {
    { (int)Mouse::LEFT, "LEFT"sv },
    { (int)Mouse::MIDDLE, "MIDDLE"sv },
    { (int)Mouse::RIGHT, "RIGHT"sv },
    { (int)Mouse::_4, "_4"sv },
    { (int)Mouse::_5, "_5"sv },
};

static constexpr Script::Record g_tableKeyboard[] = {
    { (int)Keyboard::ESC, "ESC"sv },
    { (int)Keyboard::F1, "F1"sv },
    { (int)Keyboard::F2, "F2"sv },
    { (int)Keyboard::F3, "F3"sv },
    { (int)Keyboard::F4, "F4"sv },
    { (int)Keyboard::F5, "F5"sv },
    { (int)Keyboard::F6, "F6"sv },
    { (int)Keyboard::F7, "F7"sv },
    { (int)Keyboard::F8, "F8"sv },
    { (int)Keyboard::F9, "F9"sv },
    { (int)Keyboard::F10, "F10"sv },
    { (int)Keyboard::F11, "F11"sv },
    { (int)Keyboard::F12, "F12"sv },

    { (int)Keyboard::GRAVE, "GRAVE"sv },
    { (int)Keyboard::_1, "_1"sv },
    { (int)Keyboard::_2, "_2"sv },
    { (int)Keyboard::_3, "_3"sv },
    { (int)Keyboard::_4, "_4"sv },
    { (int)Keyboard::_5, "_5"sv },
    { (int)Keyboard::_6, "_6"sv },
    { (int)Keyboard::_7, "_7"sv },
    { (int)Keyboard::_8, "_8"sv },
    { (int)Keyboard::_9, "_9"sv },
    { (int)Keyboard::_0, "_0"sv },
    { (int)Keyboard::MINUS, "MINUS"sv },
    { (int)Keyboard::EQUAL, "EQUAL"sv },
    { (int)Keyboard::BACKSPACE, "BACKSPACE"sv },

    { (int)Keyboard::TAB, "TAB"sv },
    { (int)Keyboard::Q, "Q"sv },
    { (int)Keyboard::W, "W"sv },
    { (int)Keyboard::E, "E"sv },
    { (int)Keyboard::R, "R"sv },
    { (int)Keyboard::T, "T"sv },
    { (int)Keyboard::Y, "Y"sv },
    { (int)Keyboard::U, "U"sv },
    { (int)Keyboard::I, "I"sv },
    { (int)Keyboard::O, "O"sv },
    { (int)Keyboard::P, "P"sv },
    { (int)Keyboard::LBRACE, "LBRACE"sv },
    { (int)Keyboard::RBRACE, "RBRACE"sv },
    { (int)Keyboard::BACKSLASH, "BACKSLASH"sv },

    { (int)Keyboard::CAPSLOCK, "CAPSCLOCK"sv },
    { (int)Keyboard::A, "A"sv },
    { (int)Keyboard::S, "S"sv },
    { (int)Keyboard::D, "D"sv },
    { (int)Keyboard::F, "F"sv },
    { (int)Keyboard::G, "G"sv },
    { (int)Keyboard::H, "H"sv },
    { (int)Keyboard::J, "J"sv },
    { (int)Keyboard::K, "K"sv },
    { (int)Keyboard::L, "L"sv },
    { (int)Keyboard::SEMICOLON, "SEMICOLON"sv },
    { (int)Keyboard::APOSTROPHE, "APOSTROPHE"sv },
    { (int)Keyboard::ENTER, "ENTER"sv },

    { (int)Keyboard::LSHIFT, "LSHIFT"sv },
    { (int)Keyboard::Z, "Z"sv },
    { (int)Keyboard::X, "X"sv },
    { (int)Keyboard::C, "C"sv },
    { (int)Keyboard::V, "V"sv },
    { (int)Keyboard::B, "B"sv },
    { (int)Keyboard::N, "N"sv },
    { (int)Keyboard::M, "M"sv },
    { (int)Keyboard::COMMA, "COMMA"sv },
    { (int)Keyboard::DOT, "DOT"sv },
    { (int)Keyboard::SLASH, "SLASH"sv },
    { (int)Keyboard::RSHIFT, "RSHIFT"sv },

    { (int)Keyboard::LCTRL, "LCTRL"sv },
    { (int)Keyboard::LMETA, "LMETA"sv },
    { (int)Keyboard::LALT, "LALT"sv },
    { (int)Keyboard::SPACE, "SPACE"sv },
    { (int)Keyboard::RALT, "RALT"sv },
    { (int)Keyboard::RMETA, "RMETA"sv },
    { (int)Keyboard::MENU, "MENU"sv },
    { (int)Keyboard::RCTRL, "RCTRL"sv },

    { (int)Keyboard::PRTSC, "PRTSC"sv },
    { (int)Keyboard::SCRLK, "SCRLK"sv },
    { (int)Keyboard::PAUSE, "PAUSE"sv },
    { (int)Keyboard::INSERT, "INSERT"sv },
    { (int)Keyboard::HOME, "HOME"sv },
    { (int)Keyboard::PGUP, "PGUP"sv },
    { (int)Keyboard::DELETE, "DELETE"sv },
    { (int)Keyboard::END, "END"sv },
    { (int)Keyboard::PGDW, "PGDW"sv },

    { (int)Keyboard::UP, "UP"sv },
    { (int)Keyboard::LEFT, "LEFT"sv },
    { (int)Keyboard::DOWN, "DOWN"sv },
    { (int)Keyboard::RIGHT, "RIGHT"sv },

    { (int)Keyboard::KP_NUMLK, "KP_NUMLK"sv },
    { (int)Keyboard::KP_DIV, "DIV"sv },
    { (int)Keyboard::KP_MUL, "MUL"sv },
    { (int)Keyboard::KP_MINUS, "MINUS"sv },
    { (int)Keyboard::KP_7, "KP_7"sv },
    { (int)Keyboard::KP_8, "KP_8"sv },
    { (int)Keyboard::KP_9, "KP_9"sv },
    { (int)Keyboard::KP_PLUS, "KP_PLUS"sv },
    { (int)Keyboard::KP_4, "KP_4"sv },
    { (int)Keyboard::KP_5, "KP_5"sv },
    { (int)Keyboard::KP_6, "KP_6"sv },
    { (int)Keyboard::KP_1, "KP_1"sv },
    { (int)Keyboard::KP_2, "KP_2"sv },
    { (int)Keyboard::KP_3, "KP_3"sv },
    { (int)Keyboard::KP_ENTER, "KP_ENTER"sv },
    { (int)Keyboard::KP_0, "KP_0"sv },
    { (int)Keyboard::KP_DOT, "KP_DOT"sv },
};
