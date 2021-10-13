cmake_minimum_required(VERSION 3.9.0)
project(WinnerLap VERSION 0.0.0.001)

set(CMAKE_SUPPRESS_REGENERATION true)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif()

add_executable(WinnerLap source.cpp)
