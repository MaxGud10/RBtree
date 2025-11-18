enable_testing()

# Conan генерит config-пакет, поэтому лучше так:
find_package(GTest REQUIRED CONFIG)

add_executable(rbtree_unit_tests
    unit/rbtree_unit_tests.cpp
)

target_include_directories(rbtree_unit_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

# ✅ Правильные таргеты GTest и без rb_tree
target_link_libraries(rbtree_unit_tests
    PRIVATE
        GTest::gtest_main   # или gtest::gtest_main, если вдруг так зовётся
        pthread             # под Linux обычно нужен
)

target_compile_definitions(rbtree_unit_tests PRIVATE
    TEST_DATA_DIR="${CMAKE_SOURCE_DIR}/tests"
)

add_test(NAME unit_all
  COMMAND $<TARGET_FILE:rbtree_unit_tests>
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_test(NAME e2e_small
  COMMAND bash -c "$<TARGET_FILE:rb_tree> < ${CMAKE_SOURCE_DIR}/tests/end2end/small_input.txt | diff -u - ${CMAKE_SOURCE_DIR}/tests/end2end/small_expected.txt"
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_test(NAME e2e_big_runs
  COMMAND bash -c "$<TARGET_FILE:rb_tree> < ${CMAKE_SOURCE_DIR}/tests/end2end/big_input.txt > /dev/null"
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
