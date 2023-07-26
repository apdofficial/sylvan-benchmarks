macro(add_benchmark NAME SOURCE)
    add_executable(${NAME} ${SOURCE})
    set_target_properties(${NAME} PROPERTIES COMPILE_OPTIONS "-Wno-unused")
    target_include_directories(${NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
    target_link_libraries(${NAME} PRIVATE sylvan::sylvan)
    target_compile_features(${NAME} PRIVATE c_std_11 cxx_std_11)
    target_compile_options(${NAME} PRIVATE -Wall -Wextra -Werror -Wno-deprecated)
    target_compile_options(${NAME} PRIVATE -Wno-unused-parameter)
endmacro(add_benchmark)

add_benchmark(aiger_synt aiger_synt.cpp)
target_sources(aiger_synt PRIVATE aag.h)

# Check if we have Boost
find_package(Boost 1.71.0)
if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(aiger_synt ${Boost_LIBRARIES})
endif()

# Do we want to use chaining implementation or linear probing implementation?
option(SYLVAN_USE_LINEAR_PROBING "Unique table uses chaining instead of linear probing" OFF)
if(SYLVAN_USE_LINEAR_PROBING)
    set_target_properties(aiger_synt PROPERTIES COMPILE_DEFINITIONS "SYLVAN_USE_LINEAR_PROBING")
endif ()


set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
set(CMAKE_OSX_ARCHITECTURES "arm64")