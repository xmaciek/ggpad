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
#include <limits>
#include <list>
#include <string>

#include "macros.hpp"

class Gamepad {
    DISABLE_COPY( Gamepad )

public:
    typedef int16_t value_type;
    enum Button : int16_t {
        unknown
#define MAKE_ENUM( E ) , E
#include "button_enum.def"
#undef MAKE_ENUM
        , max
    };

    struct RuntimeId {
        int32_t value = std::numeric_limits<int32_t>::max();

        constexpr bool operator == ( RuntimeId rid ) noexcept
        {
            return value == rid.value;
        }
    };

    struct [[gnu::packed]] Event {
        Button button{};
        value_type value{};
        constexpr static value_type min() { return std::numeric_limits<value_type>::min() + 1; }
        constexpr static value_type max() { return std::numeric_limits<value_type>::max(); }
        // native representation
        int16_t _type{};
        int16_t _code{};
        int32_t _value{};

        Event() noexcept = default;
        inline Event( Button b, value_type v ) noexcept
        : button{ b }
        , value{ v }
        {
        }

    };

    Gamepad() = default;
    virtual ~Gamepad() = default;

    virtual uint32_t vidpid() const = 0;
    virtual uint64_t uid() const = 0;
    virtual RuntimeId runtimeId() const = 0;
    virtual bool isConnected() const = 0;
    virtual std::string displayName() const = 0;
    virtual void disconnect() {}
};
