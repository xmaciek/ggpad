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

#include "lua_script.hpp"

int main( int argc, char** argv )
{
    const std::vector<LuaScript::Record> GAMEPAD_TABLE{
        { "X", 4 }
        , { "Y", 3 }
        , { "A", 2 }
        , { "B", 1 }
    };
    LuaScript script;
    script.bindTable( "Gamepad", GAMEPAD_TABLE );
    script.doFile( "test1.lua" );
    script.call( "GGPAD_buttonChanged" ) << 4 << 1;
    script.call( "GGPAD_buttonChanged" ) << 4 << 0;

    return 0;
}

