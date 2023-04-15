add_library(nanotrav STATIC)
add_library(nanotrav::nanotrav ALIAS nanotrav)

target_include_directories(nanotrav PUBLIC .)

target_sources(nanotrav
        PUBLIC
        ntr.c
        ntr.h
        ntrBddTest.c
        ntrHeap.c
        ntrMflow.c
        ntrShort.c
        ntrZddTest.c
        chkMterm.c
        bnet.h
        bnet.c
)