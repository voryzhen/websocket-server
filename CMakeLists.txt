cmake_minimum_required( VERSION 3.19 )
project( boost_server )

set( CMAKE_CXX_STANDARD 20 )

include_directories( C:/Users/vryzhenk/Downloads/boost_1_79_0 )
include_directories( source/include )

add_executable(

        boost_server

        source/main.cpp

)