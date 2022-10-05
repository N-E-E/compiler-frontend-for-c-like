cmake_minimum_required(VERSION 3.22)
project(my_tiny_compiler)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_EXE_LINKER_FLAGS -static)

add_executable(my_tiny_compiler main.cpp get_token.cpp get_token.h syntax_analyse.cpp syntax_analyse.h format_operation.cpp format_operation.h)
