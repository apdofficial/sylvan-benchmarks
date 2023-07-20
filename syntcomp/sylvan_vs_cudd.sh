MODEL_PATH="models"
ADD=$MODEL_PATH/"toy_examples"
DRV=$MODEL_PATH/"driver"

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

mv ./dist-newstyle/build/aarch64-osx/ghc-9.2.7/solver-0.1.0.0/x/cudd-solver/build/cudd-solver/cudd-solver .
mv ./dist-newstyle/build/aarch64-osx/ghc-9.2.7/solver-0.1.0.0/x/sylvan-solver/build/sylvan-solver/sylvan-solver .

chmod +x cudd-solver
chmod +x sylvan-solver
TEST_MODEL=$ADD/add14y.aag
time ./sylvan-solver $TEST_MODEL
time ./cudd-solver $TEST_MODEL

#hyperfine -L benchmark $ADD/add8y.aag,$ADD/add12y.aag,$ADD/add16y.aag,$ADD/add20y.aag,$ADD/add24y.aag,$ADD/add28y.aag,$ADD/add32y.aag --warmup 1 './sylvan-solver {benchmark}'
#hyperfine -L benchmark $ADD/add12y.aag,$ADD/add20y.aag,$ADD/add28y.aag,$DRV/driver_d10y.aag,$DRV/driver_d8y.aag,$DRV/driver_d6y.aag -L solver sylvan-solver,cudd-solver --warmup 1 './{solver} {benchmark}' --export-json sylvan_vs_cudd.json
#hyperfine -L benchmark $ADD/add12y.aag,$ADD/add20y.aag,$ADD/add28y.aag -L solver sylvan-solver,cudd-solver --warmup 1 './{solver} {benchmark}' --export-json sylvan_vs_cudd.json
