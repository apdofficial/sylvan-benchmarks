MODEL_PATH="/Users/andrejpistek/Developer/MSc/sylvan-benchmarks/safety_synthesis/models"
ADD=$MODEL_PATH/"toy_examples"
MUL=$MODEL_PATH/"mult_matrix"

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

echo "sylvan:add10y"
time ./sylvan-solver $ADD/add10y.aag
echo "cudd:add10y"
time ./cudd-solver $ADD/add10y.aag

echo "sylvan:add12y"
time ./sylvan-solver $ADD/add12y.aag
echo "cudd:add12y"
time ./cudd-solver $ADD/add12y.aag

echo "sylvan:add14y"
time ./sylvan-solver $ADD/add14y.aag
echo "cudd:add14y"
time ./cudd-solver $ADD/add14y.aag

echo "sylvan:mult_bool_matrix_2_3_4"
time ./sylvan-solver $MUL/mult_bool_matrix_2_3_4.aag
echo "cudd:mult_bool_matrix_2_3_4"
time ./cudd-solver $MUL/mult_bool_matrix_2_3_4.aag

echo "sylvan:mult_bool_matrix_2_3_5"
time ./sylvan-solver $MUL/mult_bool_matrix_2_3_5.aag
echo "cudd:mult_bool_matrix_2_3_5"
time ./cudd-solver $MUL/mult_bool_matrix_2_3_5.aag

echo "sylvan:mult_bool_matrix_2_3_6"
time ./sylvan-solver $MUL/mult_bool_matrix_2_3_6.aag
echo "cudd:mult_bool_matrix_2_3_6"
time ./cudd-solver $MUL/mult_bool_matrix_2_3_6.aag


hyperfine -L benchmark $ADD/add12y.aag,$ADD/add16y.aag,$ADD/add20y.aag,$ADD/add24y.aag --warmup 1 './sylvan-solver {benchmark}'
hyperfine -L benchmark $ADD/add12y.aag,$ADD/add16y.aag,$ADD/add20y.aag,$ADD/add24y.aag --warmup 1 './cudd-solver'' {benchmark}'

hyperfine -L benchmark $MUL/mult_bool_matrix_2_3_3.aag,$MUL/mult_bool_matrix_2_3_4.aag,$MUL/mult_bool_matrix_2_3_5.aag --warmup 1 './sylvan-solver {benchmark}'
hyperfine -L benchmark $MUL/mult_bool_matrix_2_3_3.aag,$MUL/mult_bool_matrix_2_3_4.aag,$MUL/mult_bool_matrix_2_3_5.aag --warmup 1 './cudd-solver {benchmark}'