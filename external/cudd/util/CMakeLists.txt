add_library(util STATIC)
add_library(util::util ALIAS util)

target_include_directories(util PUBLIC .)

target_sources(util
        PUBLIC
        config.h
        util.h
        cpu_stats.c
        cpu_time.c
        cstringstream.c
        cstringstream.h
        datalimit.c
        pathsearch.c
        pipefork.c
        prtime.c
        safe_mem.c
        strsav.c
        texpand.c
        ucbqsort.c
)