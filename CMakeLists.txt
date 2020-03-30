cmake_minimum_required(VERSION 3.0.0)
project(big_integer_tests VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

if (MSVC)
    add_compile_options(/nologo /W4 /EHsc /Ot /Ox /Ob2 /Oy /Oi)
else ()
    add_compile_options(-Wall -Wextra -Wpedantic -O3 -Ofast)
endif ()

include(CTest)
enable_testing()

set(headers
        include/catch.hpp
        include/stl_helper_functions.hpp
        include/big_integer.h
        )

set(sources_library
        src/big_integer.cpp
        )

set(sources_test
        test/main_catch_tests.cpp
        )

add_library(bi_lib OBJECT ${sources_library})
add_library(big_integer_library STATIC $<TARGET_OBJECTS:bi_lib>)

add_executable(big_integer_tests ${headers} ${sources_test})
target_link_libraries(big_integer_tests big_integer_library)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
