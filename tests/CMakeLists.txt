enable_testing()

find_package(GTest REQUIRED CONFIG)
find_package(Python3 COMPONENTS Interpreter REQUIRED)


add_executable(rbtree_unit_tests
    unit/rbtree_unit_tests.cpp
)

target_include_directories(rbtree_unit_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(rbtree_unit_tests
    PRIVATE
        GTest::gtest_main   
        pthread             
)


target_compile_definitions(rbtree_unit_tests PRIVATE TEST_DATA_DIR="${CMAKE_SOURCE_DIR}/tests")

add_test(NAME unit_all
  COMMAND $<TARGET_FILE:rbtree_unit_tests>
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_test(NAME e2e_small
  COMMAND
    ${Python3_EXECUTABLE}
    ${CMAKE_SOURCE_DIR}/tests/end2end/run_e2e.py
    --mode compare
    $<TARGET_FILE:rb_tree>
    ${CMAKE_SOURCE_DIR}/tests/end2end/small_input.txt
    ${CMAKE_SOURCE_DIR}/tests/end2end/small_expected.txt
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_test(NAME e2e_big_runs
  COMMAND
    ${Python3_EXECUTABLE}
    ${CMAKE_SOURCE_DIR}/tests/end2end/run_e2e.py
    --mode bench
    $<TARGET_FILE:rb_tree>
    ${CMAKE_SOURCE_DIR}/tests/end2end/big_input.txt
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
