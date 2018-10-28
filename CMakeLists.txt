cmake_minimum_required( VERSION 3.0.0 )
project( ggpad )
add_subdirectory( src )

configure_file( test1.lua test1.lua COPYONLY )
