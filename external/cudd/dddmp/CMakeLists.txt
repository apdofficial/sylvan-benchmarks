add_library(dddmp STATIC)
add_library(dddmp::dddmp ALIAS dddmp)

target_include_directories(dddmp PUBLIC .)

target_link_libraries(dddmp PRIVATE
        util::util
        st::st
        mtr::mtr
        epd::epd
        cudd::cudd

)

target_sources(dddmp
        PUBLIC
        dddmpStoreAdd.c
        dddmpInt.h
        dddmpStoreBdd.c
        dddmpNodeAdd.c
        dddmpNodeCnf.c
        dddmpStoreMisc.c
        dddmpLoad.c
        dddmpBinary.c
        dddmpStoreCnf.c
        dddmpLoadCnf.c
        dddmp.h
        dddmpConvert.c
        dddmpUtil.c
        dddmpNodeBdd.c
        dddmpDbg.c
)