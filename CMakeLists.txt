cmake_minimum_required(VERSION 3.11)
project("RBtree")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(tree_srcs
    main.cpp
)

option(SET_MODE_ENABLED "Enable SET mode" OFF)
set(CUSTOM_SET_VALUE "" CACHE STRING "Execution mode")

if(SET_MODE_ENABLED)
    message(STATUS "SET mode enabled")
else()
    message(STATUS "SET mode not enabled")
endif()

if(CUSTOM_SET_VALUE STREQUAL "DEBUG")
    message(STATUS "Custom mode: DEBUG")
    add_compile_definitions(CUSTOM_MODE_DEBUG)
elseif(CUSTOM_SET_VALUE STREQUAL "RELEASE")
    message(STATUS "Custom mode: RELEASE")
    add_compile_definitions(CUSTOM_MODE_RELEASE)
else()
    message(STATUS "Custom mode: Default")
endif()

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O2")
set(FLAGS "-DNDEBUG")

add_executable(rb_tree.out ${tree_srcs})
target_include_directories(rb_tree.out PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(rb_tree.out PRIVATE $<$<BOOL:${SET_MODE_ENABLED}>:SET_MODE_ENABLED>)
target_compile_options(rb_tree.out PRIVATE "$<$<CONFIG:RELEASE>:${CMAKE_CXX_FLAGS_RELEASE}>" ${FLAGS})


enable_testing()
add_subdirectory(tests)
