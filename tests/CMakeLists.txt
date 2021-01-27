add_subdirectory(google-test)

# enable CTest testing
enable_testing()

# Add a testing executable
add_executable(fp_tests
        ConversionTest.cpp
        SignTest.cpp
        ComparisonTest.cpp
        ArithmeticTest1.cpp
        ArithmeticTest2.cpp
        )
include_directories(fp_tests ../include)

target_link_libraries(fp_tests
        GTest::GTest
        GTest::Main
        )
