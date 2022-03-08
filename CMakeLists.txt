cmake_minimum_required(VERSION 3.21)
project(lang)

set(CMAKE_CXX_STANDARD 20)

add_executable(lang main.cpp tokeniser.cpp tokeniser.hpp astnode.hpp parser.cpp parser.hpp)
