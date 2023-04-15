cmake_minimum_required(VERSION 3.14)

project(sylvan-benchmarks
    VERSION 0.1
    DESCRIPTION "Sylvan dynamic variable reordering benchmarks"
    HOMEPAGE_URL "https://github.com/apdofficial/sylvan-benchmarks"
    LANGUAGES C CXX
)

add_subdirectory(sylvan)
add_subdirectory(cudd)
add_subdirectory(hashmap_cmp)
add_subdirectory(safety_synthesis)
add_subdirectory(sylvancudd_cmp)

