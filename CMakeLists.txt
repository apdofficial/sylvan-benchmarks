cmake_minimum_required(VERSION 3.14)

project(sylvan-benchmarks
    VERSION 0.1
    DESCRIPTION "Sylvan dynamic variable reordering benchmarks"
    HOMEPAGE_URL "https://github.com/apdofficial/sylvan-benchmarks"
    LANGUAGES C CXX
)

add_subdirectory(external/sylvan)
add_subdirectory(external/cudd)

add_subdirectory(cmp_hashmap)
add_subdirectory(cmp_sylvancudd)
add_subdirectory(safety_synthesis)

