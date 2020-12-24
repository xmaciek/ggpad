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

#include <array>

#include <linux/input-event-codes.h>

#include "keyboard.hpp"

static constexpr auto makeKeyButtonLUT() noexcept
{
    std::array<uint16_t, (uint32_t)Mouse::end> ret{};
    ret[ (uint32_t)Keyboard::ESC ] = KEY_ESC;
    ret[ (uint32_t)Keyboard::F1 ] = KEY_F1;
    ret[ (uint32_t)Keyboard::F2 ] = KEY_F2;
    ret[ (uint32_t)Keyboard::F3 ] = KEY_F3;
    ret[ (uint32_t)Keyboard::F4 ] = KEY_F4;
    ret[ (uint32_t)Keyboard::F5 ] = KEY_F5;
    ret[ (uint32_t)Keyboard::F6 ] = KEY_F6;
    ret[ (uint32_t)Keyboard::F7 ] = KEY_F7;
    ret[ (uint32_t)Keyboard::F8 ] = KEY_F8;
    ret[ (uint32_t)Keyboard::F9 ] = KEY_F9;
    ret[ (uint32_t)Keyboard::F10 ] = KEY_F10;
    ret[ (uint32_t)Keyboard::F11 ] = KEY_F11;
    ret[ (uint32_t)Keyboard::F12 ] = KEY_F12;

    ret[ (uint32_t)Keyboard::GRAVE ] = KEY_GRAVE;
    ret[ (uint32_t)Keyboard::_1 ] = KEY_1;
    ret[ (uint32_t)Keyboard::_2 ] = KEY_2;
    ret[ (uint32_t)Keyboard::_3 ] = KEY_3;
    ret[ (uint32_t)Keyboard::_4 ] = KEY_4;
    ret[ (uint32_t)Keyboard::_5 ] = KEY_5;
    ret[ (uint32_t)Keyboard::_6 ] = KEY_6;
    ret[ (uint32_t)Keyboard::_7 ] = KEY_7;
    ret[ (uint32_t)Keyboard::_8 ] = KEY_8;
    ret[ (uint32_t)Keyboard::_9 ] = KEY_9;
    ret[ (uint32_t)Keyboard::_0 ] = KEY_0;
    ret[ (uint32_t)Keyboard::MINUS ] = KEY_MINUS;
    ret[ (uint32_t)Keyboard::EQUAL ] = KEY_EQUAL;
    ret[ (uint32_t)Keyboard::BACKSPACE ] = KEY_BACKSPACE;

    ret[ (uint32_t)Keyboard::TAB ] = KEY_TAB;
    ret[ (uint32_t)Keyboard::Q ] = KEY_Q;
    ret[ (uint32_t)Keyboard::W ] = KEY_W;
    ret[ (uint32_t)Keyboard::E ] = KEY_E;
    ret[ (uint32_t)Keyboard::R ] = KEY_R;
    ret[ (uint32_t)Keyboard::T ] = KEY_T;
    ret[ (uint32_t)Keyboard::Y ] = KEY_Y;
    ret[ (uint32_t)Keyboard::U ] = KEY_U;
    ret[ (uint32_t)Keyboard::I ] = KEY_I;
    ret[ (uint32_t)Keyboard::O ] = KEY_O;
    ret[ (uint32_t)Keyboard::P ] = KEY_P;
    ret[ (uint32_t)Keyboard::LBRACE ] = KEY_LEFTBRACE;
    ret[ (uint32_t)Keyboard::RBRACE ] = KEY_RIGHTBRACE;
    ret[ (uint32_t)Keyboard::BACKSLASH ] = KEY_BACKSLASH;

    ret[ (uint32_t)Keyboard::CAPSLOCK ] = KEY_CAPSLOCK;
    ret[ (uint32_t)Keyboard::A ] = KEY_A;
    ret[ (uint32_t)Keyboard::S ] = KEY_S;
    ret[ (uint32_t)Keyboard::D ] = KEY_D;
    ret[ (uint32_t)Keyboard::F ] = KEY_F;
    ret[ (uint32_t)Keyboard::G ] = KEY_G;
    ret[ (uint32_t)Keyboard::H ] = KEY_H;
    ret[ (uint32_t)Keyboard::J ] = KEY_J;
    ret[ (uint32_t)Keyboard::K ] = KEY_K;
    ret[ (uint32_t)Keyboard::L ] = KEY_L;
    ret[ (uint32_t)Keyboard::SEMICOLON ] = KEY_SEMICOLON;
    ret[ (uint32_t)Keyboard::APOSTROPHE ] = KEY_APOSTROPHE;
    ret[ (uint32_t)Keyboard::ENTER ] = KEY_ENTER;

    ret[ (uint32_t)Keyboard::LSHIFT ] = KEY_LEFTSHIFT;
    ret[ (uint32_t)Keyboard::Z ] = KEY_Z;
    ret[ (uint32_t)Keyboard::X ] = KEY_X;
    ret[ (uint32_t)Keyboard::C ] = KEY_C;
    ret[ (uint32_t)Keyboard::V ] = KEY_V;
    ret[ (uint32_t)Keyboard::B ] = KEY_B;
    ret[ (uint32_t)Keyboard::N ] = KEY_N;
    ret[ (uint32_t)Keyboard::M ] = KEY_M;
    ret[ (uint32_t)Keyboard::COMMA ] = KEY_COMMA;
    ret[ (uint32_t)Keyboard::DOT ] = KEY_DOT;
    ret[ (uint32_t)Keyboard::SLASH ] = KEY_SLASH;
    ret[ (uint32_t)Keyboard::RSHIFT ] = KEY_RIGHTSHIFT;

    ret[ (uint32_t)Keyboard::LCTRL ] = KEY_LEFTCTRL;
    ret[ (uint32_t)Keyboard::LMETA ] = KEY_LEFTMETA;
    ret[ (uint32_t)Keyboard::LALT ] = KEY_LEFTALT;
    ret[ (uint32_t)Keyboard::SPACE ] = KEY_SPACE;
    ret[ (uint32_t)Keyboard::RALT ] = KEY_RIGHTALT;
    ret[ (uint32_t)Keyboard::RMETA ] = KEY_RIGHTMETA;
    ret[ (uint32_t)Keyboard::MENU ] = KEY_MENU;
    ret[ (uint32_t)Keyboard::RCTRL ] = KEY_RIGHTCTRL;

    ret[ (uint32_t)Keyboard::PRTSC ] = KEY_PRINT;
    ret[ (uint32_t)Keyboard::SCRLK ] = KEY_SCROLLLOCK;
    ret[ (uint32_t)Keyboard::PAUSE ] = KEY_PAUSE;
    ret[ (uint32_t)Keyboard::INSERT ] = KEY_INSERT;
    ret[ (uint32_t)Keyboard::HOME ] = KEY_HOME;
    ret[ (uint32_t)Keyboard::PGUP ] = KEY_PAGEUP;
    ret[ (uint32_t)Keyboard::DELETE ] = KEY_DELETE;
    ret[ (uint32_t)Keyboard::END ] = KEY_END;
    ret[ (uint32_t)Keyboard::PGDW ] = KEY_PAGEDOWN;

    ret[ (uint32_t)Keyboard::UP ] = KEY_UP;
    ret[ (uint32_t)Keyboard::LEFT ] = KEY_LEFT;
    ret[ (uint32_t)Keyboard::DOWN ] = KEY_DOWN;
    ret[ (uint32_t)Keyboard::RIGHT ] = KEY_RIGHT;

    ret[ (uint32_t)Keyboard::KP_NUMLK ] = KEY_NUMLOCK;
    ret[ (uint32_t)Keyboard::KP_DIV ] = KEY_KPSLASH;
    ret[ (uint32_t)Keyboard::KP_MUL ] = KEY_KPASTERISK;
    ret[ (uint32_t)Keyboard::KP_MINUS ] = KEY_MINUS;
    ret[ (uint32_t)Keyboard::KP_7 ] = KEY_KP7;
    ret[ (uint32_t)Keyboard::KP_8 ] = KEY_KP8;
    ret[ (uint32_t)Keyboard::KP_9 ] = KEY_KP9;
    ret[ (uint32_t)Keyboard::KP_PLUS ] = KEY_KPPLUS;
    ret[ (uint32_t)Keyboard::KP_4 ] = KEY_KP4;
    ret[ (uint32_t)Keyboard::KP_5 ] = KEY_KP5;
    ret[ (uint32_t)Keyboard::KP_6 ] = KEY_KP6;
    ret[ (uint32_t)Keyboard::KP_1 ] = KEY_KP1;
    ret[ (uint32_t)Keyboard::KP_2 ] = KEY_KP2;
    ret[ (uint32_t)Keyboard::KP_3 ] = KEY_KP3;
    ret[ (uint32_t)Keyboard::KP_ENTER ] = KEY_KPENTER;
    ret[ (uint32_t)Keyboard::KP_0 ] = KEY_KP0;
    ret[ (uint32_t)Keyboard::KP_DOT ] = KEY_KPDOT;

    ret[ (uint32_t)Mouse::LEFT ] = BTN_LEFT;
    ret[ (uint32_t)Mouse::RIGHT ] = BTN_RIGHT;
    ret[ (uint32_t)Mouse::MIDDLE ] = BTN_MIDDLE;
    ret[ (uint32_t)Mouse::_4 ] = BTN_BACK;
    ret[ (uint32_t)Mouse::_5 ] = BTN_FORWARD;
    return ret;
}

static constexpr std::array g_keyButtonLUT = makeKeyButtonLUT();
