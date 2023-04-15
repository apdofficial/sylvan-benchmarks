add_library(mtr STATIC)
add_library(mtr::mtr ALIAS mtr)

target_include_directories(mtr PUBLIC .)

target_link_libraries(mtr PRIVATE
        util::util
)

target_sources(mtr
        PUBLIC
        mtr.h
        mtrBasic.c
        mtrGroup.c
        mtrInt.h
)