cmake_minimum_required(VERSION 3.11)
project(RBtree LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(tree_srcs main.cpp)

option(SET_MODE_ENABLED "Enable SET mode" OFF)
set(CUSTOM_SET_VALUE "" CACHE STRING "Execution mode (DEBUG|RELEASE|BENCH|TIME)")

message(STATUS "SET mode: ${SET_MODE_ENABLED}")
message(STATUS "Custom mode: ${CUSTOM_SET_VALUE}")

add_executable(rb_tree ${tree_srcs})
target_include_directories(rb_tree PRIVATE ${CMAKE_SOURCE_DIR}/include)

#
target_compile_definitions(rb_tree PRIVATE
  $<$<BOOL:${SET_MODE_ENABLED}>:SET_MODE_ENABLED>
)

target_compile_definitions(rb_tree PRIVATE
  CUSTOM_SET_VALUE=\"$<IF:$<STREQUAL:${CUSTOM_SET_VALUE},>,DEFAULT,${CUSTOM_SET_VALUE}}\">
)

target_compile_definitions(rb_tree PRIVATE
  $<$<AND:$<BOOL:${SET_MODE_ENABLED}>,$<STREQUAL:${CUSTOM_SET_VALUE},VERIFY>>:CUSTOM_MODE_VERIFY>
  $<$<AND:$<BOOL:${SET_MODE_ENABLED}>,$<STREQUAL:${CUSTOM_SET_VALUE},BENCH>>:CUSTOM_MODE_BENCH>
  $<$<AND:$<BOOL:${SET_MODE_ENABLED}>,$<STREQUAL:${CUSTOM_SET_VALUE},DEBUG>>:CUSTOM_MODE_DEBUG>
)
#

# target_compile_definitions(rb_tree PRIVATE $<$<BOOL:${SET_MODE_ENABLED}>:SET_MODE_ENABLED>)
# target_compile_definitions(rb_tree PRIVATE CUSTOM_SET_VALUE=\"$<IF:$<STREQUAL:${CUSTOM_SET_VALUE},>,DEFAULT,${CUSTOM_SET_VALUE}}\">)

enable_testing()
add_subdirectory(tests)
