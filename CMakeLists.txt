cmake_minimum_required(VERSION 4.3)
project(SPSCQueue)

set(CMAKE_CXX_STANDARD 14)

add_executable(SPSCQueue
        main.cpp
        SPSC_Queue.hpp)