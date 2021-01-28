cmake_minimum_required(VERSION 3.12)
project(fpnum)

set(CMAKE_CXX_STANDARD 17)

if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-reorder")

    message("CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_definitions(-D_GLIBCXX_ASSERTIONS)
    endif ()
endif ()

add_subdirectory(demo)
add_subdirectory(tests)
