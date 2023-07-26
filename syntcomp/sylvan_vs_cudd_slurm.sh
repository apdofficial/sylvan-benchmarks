MODEL_PATH="models"
ADD=$MODEL_PATH/"toy_examples"
MUL="/home/s2641682/sylvan-benchmarks/sylvan-benchmarks/safety_synthesis/models/mult_matrix"
DRV="/home/s2641682/sylvan-benchmarks/sylvan-benchmarks/safety_synthesis/models/driver"

SYLVAN_INC_PATH=/home/s2641682/sylvan-benchmarks/sylvan-benchmarks/external/sylvan/src
SYLVAN_LIB_PATH=/home/s2641682/sylvan-benchmarks/sylvan-benchmarks/cmake-build-release-chaining/external/sylvan/src

LACE_INC_PATH=/home/s2641682/sylvan-benchmarks/sylvan-benchmarks/cmake-build-release-chaining/_deps/lace-src/src
LACE_LIB_PATH=/home/s2641682/sylvan-benchmarks/sylvan-benchmarks/cmake-build-release-chaining/_deps/lace-build

CUDD_INC_PATH=/home/s2641682/sylvan-benchmarks/sylvan-benchmarks/external/cudd/cudd
CUDD_LIB_PATH=/home/s2641682/sylvan-benchmarks/sylvan-benchmarks/cmake-build-release-chaining/external/cudd/cudd

cabal new-clean

cabal new-build \
    --extra-lib-dirs=${CUDD_LIB_PATH} \
    --extra-lib-dirs=${LACE_LIB_PATH} \
    --extra-lib-dirs=${SYLVAN_LIB_PATH} \
    --extra-include-dirs=${CUDD_INC_PATH} \
    --extra-include-dirs=${LACE_INC_PATH} \
    --extra-include-dirs=${SYLVAN_INC_PATH}


mv ./dist-newstyle/build/x86_64-linux/ghc-8.6.5/solver-0.1.0.0/x/cudd-solver/build/cudd-solver/cudd-solver .
mv ./dist-newstyle/build/x86_64-linux/ghc-8.6.5/solver-0.1.0.0/x/sylvan-solver/build/sylvan-solver/sylvan-solver .

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

./hyperfine -L solver cudd,sylvan -L model $ADD/add12y.aag --warmup 1 './{solver}-solver {model}'
./hyperfine -L solver cudd,sylvan -L model $ADD/add14y.aag --warmup 1 './{solver}-solver {model}'
./hyperfine -L solver cudd,sylvan -L model $ADD/add16y.aag --warmup 1 './{solver}-solver {model}'
./hyperfine -L solver cudd,sylvan -L model $ADD/add20y.aag --warmup 1 './{solver}-solver {model}'
./hyperfine -L solver cudd,sylvan -L model $ADD/add24y.aag --warmup 1 './{solver}-solver {model}'
./hyperfine -L solver cudd,sylvan -L model $ADD/add28y.aag --warmup 1 './{solver}-solver {model}'

./hyperfine -L solver cudd,sylvan -L model $MUL/mult_bool_matrix_2_3_3.aag --warmup 1 './{solver}-solver {model}'
./hyperfine -L solver cudd,sylvan -L model $MUL/mult_bool_matrix_2_3_4.aag --warmup 1 './{solver}-solver {model}'
./hyperfine -L solver cudd,sylvan -L model $MUL/mult_bool_matrix_2_3_5.aag --warmup 1 './{solver}-solver {model}'