set(UNIT_TEST_SRC
    unit_test_cpp_template.cpp
)

add_executable(unit_tests)
target_sources(unit_tests PRIVATE ${UNIT_TEST_SRC})
target_link_libraries(unit_tests PRIVATE cpp_template Catch2::Catch2WithMain)

enable_testing()

add_test(NAME unit_tests
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND unit_tests)
