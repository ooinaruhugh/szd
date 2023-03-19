cmake_minimum_required(VERSION 3.20)
project(szd)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SZD_SOURCES
    util.cpp
    zip/zip.cpp
    zip/cdr.cpp
    zip/eocdr.cpp
    zip/LocalHeader.cpp
)

add_executable(szd main.cpp ${SZD_SOURCES})

find_package(
    Boost 1.40 REQUIRED
    COMPONENTS program_options 
)
target_link_libraries(szd Boost::program_options)