cmake_minimum_required(VERSION 3.14)
project(basket_pricer_main)

set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_CXX_STANDARD 17)

add_executable(basket_pricer_main main.cpp)

# This is a trick to allow #include "app_context.h" from Google_tests/basket_config_test.cpp
include_directories(src)
add_subdirectory(src)
add_subdirectory(throughput)

target_link_libraries(basket_pricer_main basket_pricer_lib)

add_subdirectory(Google_tests)
