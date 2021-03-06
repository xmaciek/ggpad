# GGPAD Copyright 2019 Maciej Latocha
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

find_package( Lua REQUIRED )
if ( NOT TARGET Lua::Lua )
    add_library( Lua::Lua UNKNOWN IMPORTED )
    set_target_properties( Lua::Lua PROPERTIES
        IMPORTED_LOCATION  "${LUA_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${LUA_INCLUDE_DIR}"
    )
endif()
