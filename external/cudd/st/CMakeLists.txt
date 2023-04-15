add_library(st STATIC)
add_library(st::st ALIAS st)

target_include_directories(st PUBLIC .)

target_link_libraries(st PRIVATE
        util::util
)

target_sources(st
        PUBLIC
        st.c
        st.h
)