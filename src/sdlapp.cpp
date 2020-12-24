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
#include <string_view>

#include "actuator.hpp"
#include "sdl_gamepad.hpp"

static Actuator remapAxis( uint8_t axis )
{
    switch ( axis ) {
    case SDL_CONTROLLER_AXIS_LEFTX: return Actuator::LX;
    case SDL_CONTROLLER_AXIS_LEFTY: return Actuator::LY;
    case SDL_CONTROLLER_AXIS_RIGHTX: return Actuator::RX;
    case SDL_CONTROLLER_AXIS_RIGHTY: return Actuator::RY;
    case SDL_CONTROLLER_AXIS_TRIGGERLEFT: return Actuator::L2;
    case SDL_CONTROLLER_AXIS_TRIGGERRIGHT: return Actuator::R2;
    default:
        assert( !"unhandled enum" );
        return {};
    }
}

static Actuator remapButton( uint8_t button )
{
    switch ( button ) {
    case SDL_CONTROLLER_BUTTON_A: return Actuator::A;
    case SDL_CONTROLLER_BUTTON_B: return Actuator::B;
    case SDL_CONTROLLER_BUTTON_X: return Actuator::X;
    case SDL_CONTROLLER_BUTTON_Y: return Actuator::Y;
    case SDL_CONTROLLER_BUTTON_DPAD_UP: return Actuator::UP;
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN: return Actuator::DOWN;
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT: return Actuator::LEFT;
    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: return Actuator::RIGHT;
    case SDL_CONTROLLER_BUTTON_BACK: return Actuator::SELECT;
    case SDL_CONTROLLER_BUTTON_START: return Actuator::START;
    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: return Actuator::L1;
    case SDL_CONTROLLER_BUTTON_LEFTSTICK: return Actuator::L3;
    case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: return Actuator::R1;
    case SDL_CONTROLLER_BUTTON_RIGHTSTICK: return Actuator::R3;
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
    SDL_InitSubSystem( SDL_INIT_GAMECONTROLLER );
}

SDLApp::Event SDLApp::next() noexcept
{
    SDL_Event event{};
    if ( !SDL_PollEvent( &event ) ) {
        return SDLApp::SleepFor{ std::chrono::milliseconds{ 5 } };
    }

    switch ( event.type ) {
    case SDL_CONTROLLERDEVICEADDED:
        if ( !SDL_GetHintBoolean( "SDL_GAMECONTROLLER_ALLOW_STEAM_VIRTUAL_GAMEPAD", SDL_FALSE ) ) {
            const char* name = SDL_GameControllerNameForIndex( event.cdevice.which );
            const std::string_view namesv = name ? name : "";
            if ( namesv == "Steam Virtual Gamepad" ) {
                return std::monostate{};
            }
        }
        return SDLApp::Connected{ new SDLGamepad{ SDL_GameControllerOpen( event.cdevice.which ) } };

    case SDL_CONTROLLERDEVICEREMOVED:
        return SDLApp::Disconnected{ event.cdevice.which };

    case SDL_CONTROLLERBUTTONDOWN:
    case SDL_CONTROLLERBUTTONUP:
        return SDLApp::Input{
            Gamepad::RuntimeId{ event.cbutton.which },
            Gamepad::Event{
                remapButton( event.cbutton.button ),
                event.cbutton.state
            }
        };
    case SDL_CONTROLLERAXISMOTION:
        return SDLApp::Input{
            Gamepad::RuntimeId{ event.caxis.which },
            Gamepad::Event{
                remapAxis( event.caxis.axis ),
                event.caxis.value }
        };

    default:
        return std::monostate{};
    }

}

void SDLApp::update() noexcept
{
    SDL_PumpEvents();
}
