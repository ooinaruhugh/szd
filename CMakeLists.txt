cmake_minimum_required(VERSION 3.20)
project(szd)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SZD_SOURCES
    util.cpp
    memmem.c
    zip/zip.cpp
    zip/cdr.cpp
    zip/eocdr.cpp
    zip/LocalHeader.cpp
)

set(HEADERS
    util.h
    memmem.h
    zip/zip.h
    zip/cdr.h
    zip/eocdr.h
    zip/LocalHeader.h
)

add_executable(szd main.cpp ${SZD_SOURCES} ${HEADERS})

find_package(
    Boost 1.40 REQUIRED
    COMPONENTS program_options 
)
include_directories(${Boost_INCLUDE_DIRS})
target_link_libraries(szd ${Boost_PROGRAM_OPTIONS_LIBRARY})

if (WIN32)
    message("Doing some Windows specific stuff")
    # disable autolinking in boost
    add_definitions( -DBOOST_ALL_NO_LIB )

    # force all boost libraries to dynamic link (we already disabled
    # autolinking, so I don't know why we need this, but we do!)
    add_definitions( -DBOOST_ALL_DYN_LINK )
endif()