macro(add_benchmark NAME SOURCE)
    add_executable(${NAME} ${SOURCE})
    set_target_properties(${NAME} PROPERTIES COMPILE_OPTIONS "-Wno-unused")
    target_include_directories(${NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
    target_link_libraries(${NAME} PRIVATE sylvan::sylvan)
    target_sources(${NAME} PRIVATE helper_sylvan.h helper_cudd.h)
    target_link_libraries(${NAME}
            PRIVATE
            cudd::cudd
            util::util
            st::st
            mtr::mtr
            epd::epd
            dddmp::dddmp
    )
    target_compile_features(${NAME} PRIVATE c_std_11 cxx_std_11)
endmacro(add_benchmark)

add_benchmark(sylvancudd_cmp sylvancudd_cmp.c)

# Do we want to limit the probe sequence?
option(SYLVAN_LIMIT_PROBE_SEQUENCE "Let Sylvan limit number of iterations for insertion until returning error" OFF)
if(SYLVAN_LIMIT_PROBE_SEQUENCE)
    set_target_properties(sylvancudd_cmp PROPERTIES COMPILE_DEFINITIONS "SYLVAN_LIMIT_PROBE_SEQUENCE")
endif()

# Do we want to use chaining implementation or linear probing implementation?
option(SYLVAN_USE_CHAINING "Unique table uses chaining instead of linear probing" OFF)
if(SYLVAN_USE_CHAINING)
    set_target_properties(sylvancudd_cmp PROPERTIES COMPILE_DEFINITIONS "SYLVAN_USE_CHAINING")
endif ()


