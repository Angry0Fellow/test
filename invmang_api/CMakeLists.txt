cmake_minimum_required(VERSION 3.5)
project(invmang_api)

set(CMAKE_CXX_STANDARD 11)

include_directories(/usr/include/mysql-cppconn-8)
include_directories(.)
add_executable(invmang_api main.cpp)
target_link_libraries(invmang_api mysqlcppconn)
