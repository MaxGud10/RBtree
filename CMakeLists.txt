cmake_minimum_required(VERSION 3.11)
project("RB_tree")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(tree_srcs
    main.cpp
)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O2")
set(FLAGS   "-D NDEBUG")

add_executable(rb_tree.x  ${tree_srcs})

target_include_directories(rb_tree.x PRIVATE 
            ${CMAKE_SOURCE_DIR}/include
          
)

target_compile_options(rb_tree.x  PRIVATE "$<$<CONFIG:RELEASE>:${CMAKE_CXX_FLAGS_RELEASE}>" ${FLAGS})
