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

#include <cstdint>
#include <map>
#include <string>

#include "macros.hpp"

class Settings {
    DISABLE_COPY( Settings )

    std::map<uint64_t, std::string> m_gamepadsScriptFile;

public:
    Settings();
    ~Settings();

    void save();

    std::string& operator [] ( uint64_t uid );
};
