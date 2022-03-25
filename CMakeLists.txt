cmake_minimum_required(VERSION 3.21)
project(lang)

set(CMAKE_CXX_STANDARD 20)

add_executable(lang main.cpp tokeniser.cpp tokeniser.hpp astnode.hpp parser.cpp parser.hpp interpreter.cpp interpreter.hpp log.cpp log.hpp loc.hpp type.hpp type.cpp c.hpp)

add_library(jl c.cpp)
TARGET_LINK_LIBRARIES(lang jl)