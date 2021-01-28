add_executable(accuracy accuracy.cpp)
target_include_directories(accuracy PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(fft_demo fft_real.cpp)
target_include_directories(fft_demo PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(arith_map arith_map.cpp)
target_include_directories(arith_map PRIVATE ${CMAKE_SOURCE_DIR}/include)
