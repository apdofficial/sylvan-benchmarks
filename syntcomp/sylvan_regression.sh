ADD="models/toy_examples"

SYLVAN_INC_PATH=/Users/andrejpistek/Developer/MSc/sylvan-benchmarks/external/sylvan/src
SYLVAN_LIB_PATH=/Users/andrejpistek/Developer/MSc/sylvan-benchmarks/cmake-build-release-chaining/external/sylvan/src

LACE_INC_PATH=/Users/andrejpistek/Developer/MSc/sylvan-benchmarks/cmake-build-release-chaining/_deps/lace-src/src
LACE_LIB_PATH=/Users/andrejpistek/Developer/MSc/sylvan-benchmarks/cmake-build-release-chaining/_deps/lace-build

CUDD_INC_PATH=/Users/andrejpistek/Developer/MSc/sylvan-benchmarks/external/cudd/cudd
CUDD_LIB_PATH=/Users/andrejpistek/Developer/MSc/sylvan-benchmarks/cmake-build-release-chaining/external/cudd/cudd

cabal clean

cabal build \
    --extra-lib-dirs=${CUDD_LIB_PATH} \
    --extra-lib-dirs=${LACE_LIB_PATH} \
    --extra-lib-dirs=${SYLVAN_LIB_PATH} \
    --extra-include-dirs=${CUDD_INC_PATH} \
    --extra-include-dirs=${LACE_INC_PATH} \
    --extra-include-dirs=${SYLVAN_INC_PATH}

mv ./dist-newstyle/build/aarch64-osx/ghc-9.2.7/solver-0.1.0.0/x/sylvan-solver/build/sylvan-solver/sylvan-solver .
chmod +x sylvan-solver
mv sylvan-solver sylvan-solver-1

#./sylvan-solver models/toy_examples/add8y.aag
#./sylvan-solver models/toy_examples/add10y.aag
#./sylvan-solver models/toy_examples/add12y.aag
#./sylvan-solver models/toy_examples/add14y.aag

#hyperfine -L benchmark $ADD/add12y.aag -L solver sylvan-solver-1,sylvan-solver-2 --warmup 1 './{solver} {benchmark}'
#hyperfine -L benchmark $ADD/add28y.aag -L solver sylvan-solver-1,sylvan-solver-2 --warmup 1 './{solver} {benchmark}'

# sylvan-solver-1:
# S.laceStart 8 0
# S.setLimits (1 `shiftL` 26) 1 7