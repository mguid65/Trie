cmake_minimum_required(VERSION 3.16)
project(Trie LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(cmake/utilities.cmake)

enable_warnings()

option(COVERAGE "Enable code coverage flags" Off)
option(SANITIZE "Enable sanitizer flags" Off)

option(CPP_TEMPLATE_BUILD_SAMPLE "Enable building of sample" On)
option(CPP_TEMPLATE_BUILD_TESTS "Enable building of tests" On)

if (COVERAGE)
    enable_coverage()
endif ()

if (SANITIZE)
    enable_sanitizer()
endif ()

set(CPP_TEMPLATE_HEADERS
        include/Trie/Trie.hpp
)

add_library(trie INTERFACE)
target_include_directories(trie INTERFACE include)
target_sources(trie INTERFACE ${CPP_TEMPLATE_HEADERS})
set_target_properties(trie PROPERTIES LINKER_LANGUAGE CXX)

install(DIRECTORY include/ DESTINATION include)
install(TARGETS trie DESTINATION lib)

if (CPP_TEMPLATE_BUILD_SAMPLE)
    add_executable(sample)
    target_sources(sample PRIVATE source/sample/main.cpp)
    target_link_libraries(sample PRIVATE trie)
endif ()

if (CPP_TEMPLATE_BUILD_TESTS)
    include(CTest)
    enable_testing()

    if (CPP_TEMPLATE_BUILD_SAMPLE)
        add_test(NAME sample
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                COMMAND sample)
    endif ()

    find_package(Catch2 COMPONENTS Catch2WithMain)

    if (Catch2_FOUND)
        add_subdirectory(test)
    endif ()
endif ()
