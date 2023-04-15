macro(add_benchmark NAME SOURCE)
    add_executable(${NAME} ${SOURCE})
    set_target_properties(${NAME} PROPERTIES COMPILE_OPTIONS "-Wno-unused")
    target_include_directories(${NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
    target_link_libraries(${NAME} PRIVATE sylvan::sylvan)
    target_sources(${NAME} PRIVATE helper_cudd.h helper_sylvan.h)
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
    target_compile_options(${NAME} PRIVATE -Wno-deprecated -Wno-psabi -Wno-unused-parameter)
endmacro(add_benchmark)

add_benchmark(cmp_sylvancudd cmp_sylvancudd.c)

option(SYLVAN_LIMIT_PROBE_SEQUENCE "Let Sylvan limit number of iterations for insertion until returning error" OFF)
if(SYLVAN_LIMIT_PROBE_SEQUENCE)
    set_target_properties(cmp_sylvancudd PROPERTIES COMPILE_DEFINITIONS "SYLVAN_LIMIT_PROBE_SEQUENCE")
endif()

option(SYLVAN_USE_LINEAR_PROBING "Unique table uses linear probing instead of chaining" OFF)
if(SYLVAN_USE_LINEAR_PROBING)
    set_target_properties(cmp_sylvancudd PROPERTIES COMPILE_DEFINITIONS "SYLVAN_USE_LINEAR_PROBING")
endif ()


