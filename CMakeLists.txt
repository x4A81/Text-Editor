cmake_minimum_required(VERSION 3.10)

project(pen VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(pen src/main.cpp src/editor.cpp src/terminal.cpp src/gap_buffer.cpp)
target_include_directories(pen PRIVATE include)

if (MINGW)
    target_link_options(pen PRIVATE
        -static
        -static-libgcc
        -static-libstdc++
    )
endif()