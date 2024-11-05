cmake_minimum_required(VERSION 3.15)
project(test_package CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(trie REQUIRED CONFIG)

add_executable(example src/example.cpp)
target_link_libraries(example PRIVATE trie::trie)
