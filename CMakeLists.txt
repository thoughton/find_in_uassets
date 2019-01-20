cmake_minimum_required(VERSION 3.13)
project(find_in_uassets)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(find_in_uassets main.cpp)

target_link_libraries(find_in_uassets stdc++fs)
