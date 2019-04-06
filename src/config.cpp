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

#include "config.hpp"

#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <filesystem>
#include <fstream>

#include "log.hpp"
#include "script.hpp"

static std::filesystem::path configDir()
{
    const char* home = std::getenv( "HOME" );
    assert( home );
    std::filesystem::path path = home ? home : ".";
    path /= ".config";
    path /= "ggpad";
    return path;
}

static std::filesystem::path configFile()
{
    return configDir() / "config.lua";
}

Config::Config()
{
    std::filesystem::path filePath = configFile();
    if ( !std::filesystem::exists( filePath ) ) {
        LOG( LOG_ERROR, "Unable to read config file %s\n", filePath.c_str() );
        return;
    }
    Script luaScript;
    luaScript.doFile( filePath.c_str() );
    std::vector<Script::Pair> table = luaScript.getTable( "bindings" );
    LOG( LOG_DEBUG, "Number of bindings: %d\n", table.size() );
    for ( Script::Pair& it : table ) {
        m_gamepadsScriptFile[ std::get<int64_t>( it.first ) ] = std::get<std::string>( it.second );
        LOG( LOG_DEBUG, "Found binding for 0x%08X: %s\n", std::get<int64_t>( it.first ), std::get<std::string>( it.second ).c_str() );
    }
}

Config::~Config()
{
}

static std::string formatVidPid( uint64_t vidpid )
{
    char text[ 18 ];
    std::snprintf( text, 18, "0x%08X", vidpid );
    return text;
}

void Config::save()
{
    std::filesystem::create_directory( configDir() );
    std::ofstream ofs( configFile() );
    ofs << "bindings = {}" << std::endl;
    for ( const std::pair<uint64_t,std::string>& it : m_gamepadsScriptFile ) {
        ofs << "bindings[ " << formatVidPid( it.first ) << " ] = \"" << it.second << "\"" << std::endl;
    }
}

std::string& Config::operator [] ( uint64_t uid )
{
    return m_gamepadsScriptFile[ uid ];
}
