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

#include <chrono>
#include <variant>

#include "nocopy.hpp"
#include "gamepad.hpp"
#include "sdl_gamepad.hpp"

class SDLApp : nocopy {

public:
    struct SleepFor {
        std::chrono::high_resolution_clock::duration value{};
    };

    struct Connected {
        Gamepad* gamepad = nullptr;
    };

    struct Disconnected {
        Gamepad::RuntimeId id{};
    };

    struct Input {
        Gamepad::RuntimeId id{};
        Gamepad::Event value{};
    };

    using Event = std::variant< std::monostate
        , SleepFor
        , Connected
        , Disconnected
        , Input
    >;

    ~SDLApp() noexcept;
    SDLApp() noexcept;

    Event next() noexcept;
    void update() noexcept;
};
