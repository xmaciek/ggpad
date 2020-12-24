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

#include <cstdint>

enum class Keyboard : uint16_t {
    ESC,
    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,

    GRAVE,
    _1,
    _2,
    _3,
    _4,
    _5,
    _6,
    _7,
    _8,
    _9,
    _0,
    MINUS,
    EQUAL,
    BACKSPACE,

    TAB,
    Q,
    W,
    E,
    R,
    T,
    Y,
    U,
    I,
    O,
    P,
    LBRACE,
    RBRACE,
    BACKSLASH,

    CAPSLOCK,
    A,
    S,
    D,
    F,
    G,
    H,
    J,
    K,
    L,
    SEMICOLON,
    APOSTROPHE,
    ENTER,

    LSHIFT,
    Z,
    X,
    C,
    V,
    B,
    N,
    M,
    COMMA,
    DOT,
    SLASH,
    RSHIFT,

    LCTRL,
    LMETA,
    LALT,
    SPACE,
    RALT,
    RMETA,
    MENU,
    RCTRL,

    PRTSC,
    SCRLK,
    PAUSE,
    INSERT,
    HOME,
    PGUP,
    DELETE,
    END,
    PGDW,

    UP,
    LEFT,
    DOWN,
    RIGHT,

    KP_NUMLK,
    KP_DIV,
    KP_MUL,
    KP_MINUS,
    KP_7,
    KP_8,
    KP_9,
    KP_PLUS,
    KP_4,
    KP_5,
    KP_6,
    KP_1,
    KP_2,
    KP_3,
    KP_ENTER,
    KP_0,
    KP_DOT,

    end,
};

enum class Mouse : uint16_t {
    LEFT = (uint16_t)Keyboard::end,
    MIDDLE,
    RIGHT,
    _4,
    _5,
    end,
};
