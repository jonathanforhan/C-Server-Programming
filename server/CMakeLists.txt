cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(tcp_server)

add_subdirectory(src)

add_executable(${PROJECT_NAME} main.c)

target_link_libraries(${PROJECT_NAME} tcp_lib)
target_include_directories(${PROJECT_NAME} PUBLIC "./src")