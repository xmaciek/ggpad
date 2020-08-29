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

#include "settings.hpp"

#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>

#include "log.hpp"
#include "script.hpp"

static std::filesystem::path settingsDir()
{
    std::filesystem::path path{};
    const char* settingsDir = std::getenv( "XDG_CONFIG_HOME" );
    if ( settingsDir && std::strlen( settingsDir ) > 0 ) {
        path = settingsDir;
    }
    else {
        settingsDir = std::getenv( "HOME" );
        assert( settingsDir );
        path = settingsDir ? settingsDir : ".";
        path /= ".config";
    }
    path /= "ggpad";
    return path;
}

static std::filesystem::path settingsFile()
{
    return settingsDir() / "settings.lua";
}

Settings::Settings()
{
    const std::filesystem::path filePath = settingsFile();
    if ( !std::filesystem::exists( filePath ) ) {
        LOG( LOG_ERROR, "Unable to read settings file %s\n", filePath.c_str() );
        return;
    }
    Script luaScript;
    luaScript.doFile( filePath.c_str() );
    std::vector<Script::Pair> table = luaScript.getTable( "bindings" );
    LOG( LOG_DEBUG, "Number of bindings: %lu\n", table.size() );
    for ( Script::Pair& it : table ) {
        m_gamepadsScriptFile[ std::get<int64_t>( it.first ) ] = std::get<std::string>( it.second );
        LOG( LOG_DEBUG, "Found binding for 0x%08lX: %s\n", std::get<int64_t>( it.first ), std::get<std::string>( it.second ).c_str() );
    }
}

Settings::~Settings()
{
}

static std::string formatVidPid( uint64_t vidpid )
{
    char text[ 18 ];
    std::snprintf( text, 18, "0x%08lX", vidpid );
    return text;
}

void Settings::save()
{
    std::filesystem::create_directory( settingsDir() );
    std::ofstream ofs( settingsFile() );
    ofs << "bindings = {}" << std::endl;
    for ( const auto& it : m_gamepadsScriptFile ) {
        ofs << "bindings[ " << formatVidPid( it.first ) << " ] = \"" << it.second.native() << "\"" << std::endl;
    }
}

std::filesystem::path& Settings::operator [] ( uint64_t uid )
{
    return m_gamepadsScriptFile[ uid ];
}
