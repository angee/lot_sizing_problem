cmake_minimum_required(VERSION 3.5)
project(lot_sizing_problem)

set(CMAKE_CXX_STANDARD 11)

add_executable(lotsizing main.cpp lotsizing_instance.cpp lotsizing_instance.h)
target_link_libraries(lotsizing gecodeminimodel gecodedriver gecodegist gecodesearch gecodeint gecodekernel gecodesupport)
