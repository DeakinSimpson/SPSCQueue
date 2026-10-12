cmake_minimum_required(VERSION 4.3)
project(SPSCQueue)

set(CMAKE_CXX_STANDARD 26)

add_executable(SPSCQueue
        main.cpp
        include/SPSC_Queue.hpp)

# --- add the google benchmark ---
# get the content
include(FetchContent)
FetchContent_Declare(
        googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG v1.9.1
)

FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip)

# ignore googles own testing
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
# ignore WERROR for google as it was causing compile errors on my windows device
set(BENCHMARK_ENABLE_WERROR OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googlebenchmark googletest)

add_executable(queue_bench
        benchmarks/benchmark.cpp)
target_include_directories(queue_bench PRIVATE include)
target_link_libraries(queue_bench PRIVATE benchmark::benchmark)

# --- add google testing ---
add_executable(queue_tests
        tests/test.cpp)
target_include_directories(queue_tests PRIVATE include)
target_link_libraries(queue_tests gtest_main)

# integrates gtest with ctest
enable_testing()
include(GoogleTest)
gtest_discover_tests(queue_tests)