cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_STANDARD 20)
set(BOOST_MIN_VERSION 1.75)
set(Boost_USE_STATIC_LIBS on)

project(muxer)

find_package(ICU 65.0 COMPONENTS data REQUIRED)
find_package(Boost ${BOOST_MIN_VERSION} COMPONENTS log_setup log REQUIRED)
find_package(Threads)

message("** Boost Include: ${Boost_INCLUDE_DIR}")
message("** Boost Libraries: ${Boost_LIBRARY_DIRS}")
message("** Boost Libraries: ${Boost_LIBRARIES}")
message("** ICU Libraries: ${ICU_LIBRARY}")

include_directories(${Boost_INCLUDE_DIRS})
link_libraries(${Boost_LIBRARIES} ICU::data Threads::Threads)

add_compile_options(-fPIC -stdlib=libc++)
add_link_options(-stdlib=libc++)

add_executable(muxer
        main.cpp Session.h Session.cpp
        utils/utils.h utils/utils.cpp utils/logger.h utils/logger.cpp
        muxers/HTTPMuxer.cpp muxers/Muxer.h)
