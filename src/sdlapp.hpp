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

#include "macros.hpp"
#include "gamepad.hpp"

class SDLApp {
    DISABLE_COPY( SDLApp );

public:
    struct SleepFor {
        std::chrono::high_resolution_clock::duration value{};
    };

    struct Connected {
        int32_t id = 0;
    };

    struct Disconnected {
        int32_t id = 0;
    };

    struct Input {
        int32_t id = 0;
        Gamepad::Button button{};
        int16_t value = 0;
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
