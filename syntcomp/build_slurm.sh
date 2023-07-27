SRC_PATH=/home/s2641682/sylvan-benchmarks/sylvan-benchmarks
BUILD_PATH=$SRC_PATH/cmake-build-release-chaining

SYLVAN_INC_PATH=$SRC_PATH/external/sylvan/src
SYLVAN_LIB_PATH=$BUILD_PATH/external/sylvan/src

LACE_INC_PATH=$BUILD_PATH/_deps/lace-src/src
LACE_LIB_PATH=$BUILD_PATH/_deps/lace-build

CUDD_INC_PATH=$SRC_PATH/external/cudd/cudd
CUDD_LIB_PATH=$BUILD_PATH/external/cudd/cudd

cabal new-clean

cabal new-build \
    --extra-lib-dirs=${CUDD_LIB_PATH} \
    --extra-lib-dirs=${LACE_LIB_PATH} \
    --extra-lib-dirs=${SYLVAN_LIB_PATH} \
    --extra-include-dirs=${CUDD_INC_PATH} \
    --extra-include-dirs=${LACE_INC_PATH} \
    --extra-include-dirs=${SYLVAN_INC_PATH}

mv ./dist-newstyle/build/x86_64-linux/ghc-8.6.5/solver-0.1.0.0/x/cudd-solver/build/cudd-solver/cudd-solver ./tests/
mv ./dist-newstyle/build/x86_64-linux/ghc-8.6.5/solver-0.1.0.0/x/sylvan-solver/build/sylvan-solver/sylvan-solver ./tests/