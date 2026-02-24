cmake_minimum_required(VERSION 3.16)
project(LockFreeBenchmark LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(ENABLE_TSAN "Enable ThreadSanitizer" OFF)

if(ENABLE_TSAN)
    add_compile_options(-fsanitize=thread -g)
    add_link_options(-fsanitize=thread)
endif()

# Main executable
add_executable(benchmark src/main.cpp src/benchmark.cpp)
target_include_directories(benchmark PRIVATE include)
target_link_libraries(benchmark PRIVATE pthread)

# Tests (using FetchContent for gtest)
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(queue_tests tests/queue_test.cpp)
target_include_directories(queue_tests PRIVATE include)
target_link_libraries(queue_tests PRIVATE GTest::gtest_main pthread)
add_test(NAME QueueTests COMMAND queue_tests)
