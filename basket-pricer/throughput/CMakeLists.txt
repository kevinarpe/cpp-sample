cmake_minimum_required(VERSION 3.14)
project(basket_pricer_throughput)

set(CMAKE_CXX_STANDARD 17)

add_executable(basket_pricer_throughput main.cpp)

target_link_libraries(basket_pricer_throughput basket_pricer_lib)
