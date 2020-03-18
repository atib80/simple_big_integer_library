cmake_minimum_required(VERSION 3.0.0)
project(big_integer VERSION 0.1.0)
set(CMAKE_CXX_STANDARD 17)

if (MSVC)
  add_compile_options(/W4 /EHsc /O2 /Ot /Ox /Ob2 /Oy /Oi)
else()
  add_compile_options(-Wall -Wextra -Wpedantic -std=c++17 -O2 -Ofast)
endif()

include(CTest)
enable_testing()

add_executable(big_integer
        include/stl_helper_functions.hpp
        include/big_integer.h
        src/big_integer.cpp
        include/catch.hpp
        test/main_catch_tests.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
