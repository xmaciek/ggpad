// GGPAD Copyright 2019 Maciej Latocha
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
#include <cstdlib>
#include <cstdio>

class Log {
public:
    Log() = default;
    virtual ~Log() = default;
    virtual void append( const char* ) = 0;
};

extern Log* g_log;

#define LOG_ERROR stderr
#define LOG_DEBUG stdout

#define LOG( LVL, ... ) { \
    if ( g_log ) { \
        std::array<char, 256> buff{ 0 }; \
        std::snprintf( buff.data(), buff.size(), __VA_ARGS__ ); \
        g_log->append( buff.data() ); \
    } else { \
        std::fprintf( LVL, __VA_ARGS__ ); \
    } \
}
