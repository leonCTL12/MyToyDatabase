cmake_minimum_required(VERSION 3.10)
project(MyToyDatabase)

set(CMAKE_CXX_STANDARD 17)  # Use C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_COMPILER "/opt/homebrew/bin/g++-14")  # Apple Silicon

add_executable(toydb main.cpp)