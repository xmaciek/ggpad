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

#include "sdlapp.hpp"

#include <SDL2/SDL.h>

#include <cassert>
#include <algorithm>

#include "sdl_gamepad.hpp"

static Gamepad::Button remapAxis( uint8_t axis )
{
    switch ( axis ) {
    case SDL_CONTROLLER_AXIS_LEFTX: return Gamepad::Button::LX;
    case SDL_CONTROLLER_AXIS_LEFTY: return Gamepad::Button::LY;
    case SDL_CONTROLLER_AXIS_RIGHTX: return Gamepad::Button::RX;
    case SDL_CONTROLLER_AXIS_RIGHTY: return Gamepad::Button::RY;
    case SDL_CONTROLLER_AXIS_TRIGGERLEFT: return Gamepad::Button::L2;
    case SDL_CONTROLLER_AXIS_TRIGGERRIGHT: return Gamepad::Button::R2;
    default:
        assert( !"unhandled enum" );
        return {};
    }
}

static Gamepad::Button remapButton( uint8_t button )
{
    switch ( button ) {
    case SDL_CONTROLLER_BUTTON_A: return Gamepad::Button::A;
    case SDL_CONTROLLER_BUTTON_B: return Gamepad::Button::B;
    case SDL_CONTROLLER_BUTTON_X: return Gamepad::Button::X;
    case SDL_CONTROLLER_BUTTON_Y: return Gamepad::Button::Y;
    case SDL_CONTROLLER_BUTTON_DPAD_UP: return Gamepad::Button::UP;
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN: return Gamepad::Button::DOWN;
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT: return Gamepad::Button::LEFT;
    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: return Gamepad::Button::RIGHT;
    case SDL_CONTROLLER_BUTTON_BACK: return Gamepad::Button::SELECT;
    case SDL_CONTROLLER_BUTTON_START: return Gamepad::Button::START;
    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: return Gamepad::Button::L1;
    case SDL_CONTROLLER_BUTTON_LEFTSTICK: return Gamepad::Button::L3;
    case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return Gamepad::Button::R1;
    case SDL_CONTROLLER_BUTTON_RIGHTSTICK: return Gamepad::Button::R3;
    default:
        assert( !"unhandled enum" );
        return {};
    }
}

SDLApp::~SDLApp() noexcept
{
    SDL_QuitSubSystem( SDL_INIT_GAMECONTROLLER );
}

SDLApp::SDLApp() noexcept
{
    [[maybe_unused]]
    const bool b = SDL_SetHint( "SDL_GAMECONTROLLER_ALLOW_STEAM_VIRTUAL_GAMEPAD", "0" );
    assert( b );
    SDL_InitSubSystem( SDL_INIT_GAMECONTROLLER );
}

SDLApp::Event SDLApp::next() noexcept
{
    SDL_Event event{};
    if ( !SDL_PollEvent( &event ) ) {
        return SDLApp::SleepFor{ std::chrono::milliseconds{ 5 } };
    }

    switch ( event.type ) {
    case SDL_CONTROLLERDEVICEADDED: {
        SDL_GameController* gamepad = SDL_GameControllerOpen( event.cdevice.which );
        std::string name = SDL_GameControllerName( gamepad );
        std::transform( name.begin(), name.end(), name.begin(), [](char c){ return (char)std::tolower(c); } );
        if ( name.find( "virtual" ) != std::string::npos ) {
            SDL_GameControllerClose( gamepad );
            return std::monostate{};
        }

        SDL_Joystick* joystick = SDL_GameControllerGetJoystick( gamepad );
        m_gamepads.emplace_back(
            SDL_JoystickInstanceID( joystick ),
            new SDLGamepad{ gamepad }
        );
        return SDLApp::Connected{ m_gamepads.back().m_gamepad };
    }
    case SDL_CONTROLLERDEVICEREMOVED:
        m_gamepads.erase(
            std::remove_if(
                m_gamepads.begin(),
                m_gamepads.end(),
                [which=event.cdevice.which]( const LocalGamepad& s )
                {
                    return s.m_sdlId == which;
                } ),
            m_gamepads.end()
        );
        return SDLApp::Disconnected{ event.cdevice.which };

    case SDL_CONTROLLERBUTTONDOWN:
    case SDL_CONTROLLERBUTTONUP: {
        const Gamepad::Button what = remapButton( event.cbutton.button );
        auto it = std::find_if( m_gamepads.begin(), m_gamepads.end(),
            [which = event.cbutton.which]( const LocalGamepad& s )
            {
                return s.m_sdlId == which;
            }
        );
        if ( it != m_gamepads.cend() ) {
            it->m_gamepad->push( what, event.cbutton.state );
        }
        return SDLApp::Input{ event.cbutton.which, what, event.cbutton.state };
    }
    case SDL_CONTROLLERAXISMOTION: {
        const Gamepad::Button what = remapAxis( event.caxis.axis );
        auto it = std::find_if( m_gamepads.begin(), m_gamepads.end(),
            [which = event.cbutton.which]( const LocalGamepad& s )
            {
                return s.m_sdlId == which;
            }
        );
        if ( it != m_gamepads.cend() ) {
            it->m_gamepad->push( what, event.caxis.value );
        }
        return SDLApp::Input{ event.caxis.which, what, event.caxis.value };
    }

    default:
        return std::monostate{};
    }

}

void SDLApp::update() noexcept
{
    SDL_PumpEvents();
}

SDLApp::LocalGamepad::LocalGamepad( int id, SDLGamepad* pad ) noexcept
: m_sdlId{ id }
, m_gamepad{ pad }
{
}
