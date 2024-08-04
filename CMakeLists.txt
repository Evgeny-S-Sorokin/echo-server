cmake_minimum_required(VERSION 3.25)

project(echo-server VERSION 1.0)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-Ofast")

add_subdirectory(lib)
add_subdirectory(test_client)

add_executable(echo_server main.cpp)
target_link_libraries(echo_server PUBLIC co_server)
