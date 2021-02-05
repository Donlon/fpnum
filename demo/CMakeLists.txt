
find_package(Matlab REQUIRED MAT_LIBRARY)

if (Matlab_FOUND)
    add_definitions(-DMATLAB)
    add_definitions(-DEXPORT_MAT)
    include_directories(${Matlab_INCLUDE_DIRS})
    link_libraries(${Matlab_MAT_LIBRARY})
    link_libraries(${Matlab_MX_LIBRARY})
endif ()

add_executable(accuracy accuracy.cpp)
target_include_directories(accuracy PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(fft_demo fft_real.cpp)
target_include_directories(fft_demo PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(arith_map arith_map.cpp)
target_include_directories(arith_map PRIVATE ${CMAKE_SOURCE_DIR}/include)
