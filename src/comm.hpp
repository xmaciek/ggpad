// // GGPAD Copyright 2020 Maciej Latocha ( latocha.maciek@gmail.com )
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

#include "bottleneck.hpp"
#include "queue.hpp"
#include "nocopy.hpp"

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <variant>

struct Message {
    enum struct Type : uint8_t {
        eGamepadConnected,
        eGamepadDisconnected,
        eStopScript,
        eRunScript,
        eUpdateScriptPath,
    };

    std::variant<std::monostate, std::filesystem::path, std::string> m_data{};
    uint64_t m_id = 0;
    Type m_type{};

    ~Message() noexcept = default;
    Message() noexcept = default;
    explicit Message( Type, uint64_t );
    explicit Message( Type, uint64_t, const std::string& );
    explicit Message( Type, uint64_t, const std::filesystem::path& );

    const std::string& toString() const;
    const std::filesystem::path& toPath() const;
};

class Comm : nocopy {
    Queue<Message> m_queueA{};
    Queue<Message> m_queueB{};
    Bottleneck m_bottleneckA{};
    Bottleneck m_bottleneckB{};

public:
    ~Comm() noexcept = default;
    Comm() noexcept = default;

    std::optional<Message> popFromServer();
    std::optional<Message> popFromClient();

    void pushToClient( Message&& );
    void pushToServer( Message&& );

    void notifyClient();
    void notifyServer();

    void waitForClient( const std::chrono::high_resolution_clock::duration& );
    void waitForServer( const std::chrono::high_resolution_clock::duration& );
};
