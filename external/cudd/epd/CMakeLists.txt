add_library(epd STATIC)
add_library(epd::epd ALIAS epd)

target_include_directories(epd PUBLIC .)

target_link_libraries(epd PRIVATE
        util::util
)

target_sources(epd
        PUBLIC
        epd.c
        epd.h
        epdInt.h
)