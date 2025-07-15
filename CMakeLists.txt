cmake_minimum_required(VERSION 3.10)
project(tree)

set(SOURCE_FILES
	tree.c
)

if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
	add_compile_options(-Wall -Werror -pedantic)
endif()


add_executable(tree ${SOURCE_FILES})
target_link_options(tree PRIVATE "-municode")
