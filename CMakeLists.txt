cmake_minimum_required(VERSION 3.10)
project(tree)

set(SOURCE_FILES
	src/tree.c
	linked_list/linked_list.c
	log/log.c
)

if (CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
	add_compile_options(-Wall -Werror -pedantic)
endif()


add_executable(tree ${SOURCE_FILES})
target_link_options(tree PRIVATE "-municode")


add_library(get_opt_dll SHARED libs/getopt.c)
target_compile_definitions(get_opt_dll PRIVATE EXPORTS_GETOPT)

target_link_libraries(tree PRIVATE get_opt_dll)
