cmake_minimum_required(VERSION 3.5)
project(lot_sizing_problem)

set(CMAKE_CXX_STANDARD 11)
set(EXECUTABLE_NAME lotsizing)
set(MAIN_FILE
        src/main.cpp)

set(SOURCE_FILES
        src/lotsizing.cpp
        src/lotsizing_instance.cpp
        src/static_greedy_brancher.cpp
        src/hybrid_greedy_brancher.cpp
        src/sdf_greedy_brancher.cpp
        src/static_greedy_from_period_brancher.cpp)

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(${EXECUTABLE_NAME} ${MAIN_FILE} ${SOURCE_FILES})
#add_executable(lotsizing src/main.cpp src/lotsizing_instance.cpp include/lotsizing_instance.h src/lotsizing_greedy_brancher.cpp include/lotsizing_greedy_brancher.h)
target_link_libraries(lotsizing gecodeminimodel gecodedriver gecodegist gecodesearch gecodeint gecodekernel gecodesupport)
