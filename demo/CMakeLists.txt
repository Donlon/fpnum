
find_package(Matlab REQUIRED MAT_LIBRARY)

if (Matlab_FOUND)
    add_definitions(-DMATLAB)
    add_definitions(-DEXPORT_MAT)
    add_definitions(-DSHOW_FIG)
    include_directories(${Matlab_INCLUDE_DIRS})
    link_libraries(${Matlab_MAT_LIBRARY})
    link_libraries(${Matlab_MX_LIBRARY})
    link_libraries(${Matlab_DATAARRAY_LIBRARY})
    link_libraries(${Matlab_ENGINE_LIBRARY})
    link_libraries(pthread)
else ()
    message(FATAL_ERROR "Can't find MATLAB")
endif ()

add_executable(accuracy accuracy.cpp MatFile.cpp)
target_include_directories(accuracy PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(fft_demo fft_real.cpp MatFile.cpp)
target_include_directories(fft_demo PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(arith_map arith_map.cpp MatFile.cpp)
target_include_directories(arith_map PRIVATE ${CMAKE_SOURCE_DIR}/include)
