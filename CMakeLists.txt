project(muxer)
cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost COMPONENTS log log_setup REQUIRED)

message(status "** Boost Include: ${Boost_INCLUDE_DIR}")
message(status "** Boost Libraries: ${Boost_LIBRARY_DIRS}")
message(status "** Boost Libraries: ${Boost_LIBRARIES}")

include_directories(${Boost_INCLUDE_DIRS})

add_executable(muxer
        main.cpp Session.h Session.cpp utils/utils.h utils/utils.cpp
        muxers/HTTPMuxer.cpp muxers/Muxer.h utils/logger.h utils/logger.cpp)

target_link_libraries(muxer ${Boost_LIBRARIES} pthread)

set(CMAKE_CXX_FLAGS "-fPIC")
