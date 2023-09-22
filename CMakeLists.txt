cmake_minimum_required(VERSION 3.20)
project(szd
    VERSION 0.2
    LANGUAGES CXX C
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(SZD_SOURCES
    src/util.cpp
    src/memmem.c
    src/zip/zip.cpp
    src/zip/cdr.cpp
    src/zip/eocdr.cpp
    src/zip/LocalHeader.cpp
)

set(HEADERS
    src/util.h
    src/memmem.h
    src/zip/zip.h
    src/zip/cdr.h
    src/zip/eocdr.h
    src/zip/LocalHeader.h
)

add_executable(szd src/main.cpp ${SZD_SOURCES} ${HEADERS})

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