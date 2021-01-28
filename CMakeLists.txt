cmake_minimum_required(VERSION 3.0)
project(fpnum)

set(CMAKE_CXX_STANDARD 17)

if (CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-reorder")
endif ()

add_subdirectory(demo)
add_subdirectory(tests)
