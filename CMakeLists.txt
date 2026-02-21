cmake_minimum_required(VERSION 3.11)
project(RBtree LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



set(PROJECT_INCLUDE_DIRS
  ${CMAKE_SOURCE_DIR}/include
)

option(SET_MODE_ENABLED "Enable SET verification in rb_tree" OFF)
option(RBTREE_DEBUG_DOT "Dump Graphviz (CUSTOM_MODE_DEBUG)" OFF)

message(STATUS "SET mode (rb_tree): ${SET_MODE_ENABLED}")
message(STATUS "RBTREE_DEBUG_DOT: ${RBTREE_DEBUG_DOT}")

find_package(cxxopts REQUIRED CONFIG)

add_executable(rb_tree src/main.cpp)
target_include_directories(rb_tree PRIVATE ${PROJECT_INCLUDE_DIRS})
target_link_libraries(rb_tree PRIVATE cxxopts::cxxopts)


target_compile_definitions(rb_tree PRIVATE
  $<$<BOOL:${SET_MODE_ENABLED}>:SET_MODE_ENABLED>
  $<$<OR:$<CONFIG:Debug>,$<BOOL:${RBTREE_DEBUG_DOT}>>:CUSTOM_MODE_DEBUG>
)

add_executable(rb_tree_bench src/bench.cpp)
target_include_directories(rb_tree_bench PRIVATE ${PROJECT_INCLUDE_DIRS})
target_compile_definitions(rb_tree_bench PRIVATE
  $<$<BOOL:${SET_MODE_ENABLED}>:SET_MODE_ENABLED>
  CUSTOM_MODE_BENCH
)

enable_testing()
add_subdirectory(tests)
