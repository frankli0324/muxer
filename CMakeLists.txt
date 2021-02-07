project(muxer)
cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(muxer
        main.cpp Session.h Session.cpp utils.h utils.cpp
        muxers/HTTPMuxer.cpp muxers/Muxer.h)
