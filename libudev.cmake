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

find_path( LIBUDEV_INCLUDE_DIR NAMES libudev.h )
find_library( LIBUDEV_LIBRARY NAMES udev )

set( LIBUDEV_FOUND ( LIBUDEV_INCLUDE_DIR AND LIBUDEV_LIBRARY ) )

if ( NOT LIBUDEV_FOUND )
    message( FATAL_ERROR "unable to locate libudev" )
endif()

if ( NOT TARGET Udev::Udev )
    add_library( Udev::Udev UNKNOWN IMPORTED )
    set_target_properties( Udev::Udev PROPERTIES
        IMPORTED_LOCATION  "${LIBUDEV_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${LIBUDEV_INCLUDE_DIR}"
    )
endif()

mark_as_advanced(
    LIBUDEV_FOUND
    LIBUDEV_INCLUDE_DIR
    LIBUDEV_LIBRARY
)
