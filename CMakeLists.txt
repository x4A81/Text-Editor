cmake_minimum_required(VERSION 3.10)

project(editor VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(editor main.cpp src/editor.cpp src/renderer.cpp src/terminal.cpp)
