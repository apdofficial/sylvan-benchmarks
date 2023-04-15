cmake_minimum_required(VERSION 3.14)

project(cudd_test VERSION 3.0.0 LANGUAGES C)

add_executable(main main.c)
target_link_libraries(main PRIVATE cudd::cudd)

add_subdirectory(util)
add_subdirectory(st)
add_subdirectory(mtr)
add_subdirectory(epd)
add_subdirectory(dddmp)
add_subdirectory(cudd)