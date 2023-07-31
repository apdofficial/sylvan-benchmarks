MODELS_PATH="./models"
RESULTS_PATH="./results"

RESULTS_AUTO_QUALITY=$RESULTS_PATH/"auto/quality"
RESULTS_AUTO_RUNTIME=$RESULTS_PATH/"auto/runtime"

rm -r $RESULTS_AUTO_QUALITY/*
rm -r $RESULTS_AUTO_QUALITY/*

RESULTS_MANU_ONE_QUALITY=$RESULTS_PATH/"manu-one/quality"
RESULTS_MANU_ONE_RUNTIME=$RESULTS_PATH/"manu-one/runtime"

rm -r $RESULTS_MANU_ONE_QUALITY/*
rm -r $RESULTS_MANU_ONE_RUNTIME/*

RESULTS_MANU_MANY_QUALITY=$RESULTS_PATH/"manu-many/quality"
RESULTS_MANU_MANY_RUNTIME=$RESULTS_PATH/"manu-many/runtime"

rm -r $RESULTS_MANU_MANY_QUALITY/*
rm -r $RESULTS_MANU_MANY_RUNTIME/*



# AUTOMATIC REORDERING TESTS
declare -a auto_models=(
  "add10y"
  "add12y"
  "add14y"
  "add16y"
  "add18y"
  "add20y"
  "add24y"
  "add28y"
  "mult_bool_matrix_2_3_5"
  "mult_bool_matrix_2_3_6"
  "mult_bool_matrix_2_3_7"
  "mult_bool_matrix_2_3_8"
  "mult_bool_matrix_2_3_9"
  "mult_bool_matrix_2_3_11"
  "mult_bool_matrix_2_3_12"
  "mult_bool_matrix_2_3_13"
  "demo-v5_5_REAL"
  "demo-v6_5_REAL"
  "demo-v7_2_REAL"
  "demo-v7_5_REAL"
  "demo-v8_2_REAL"
  "demo-v8_5_REAL"
  "demo-v9_2_REAL"
  "demo-v9_5_REAL"
)

## MANUAL ONE REORDERING TESTS
declare -a manu_models=(
  "add2y"
  "add4y"
  "add6y"
  "add8y"
  "add10y"
  "add12y"
  "add14y"
  "mult_bool_matrix_2_3_2"
  "mult_bool_matrix_2_3_3"
  "mult_bool_matrix_2_3_4"
  "mult_bool_matrix_2_3_5"
  "mult_bool_matrix_2_3_6"
  "mult_bool_matrix_2_3_7"
  "mult_bool_matrix_2_3_8"
  "demo-v5_2_REAL"
  "demo-v3_2_REAL"
  "demo-v3_5_REAL"
  "demo-v4_5_REAL"
  "demo-v5_5_REAL"
  "demo-v6_5_REAL"
  "demo-v7_2_REAL"
)

declare -a solvers=("cudd-solver" "sylvan-solver")

solvers_str="${solvers[*]}"
separated_solvers_str=${solvers_str// /,}

for solver in "${solvers[@]}"
do
  chmod +x "$solver"
done

# measure quality of the semi-automatic reordering
for model in "${auto_models[@]}"
do
  ./sylvan-solver --threads 1 --nodes_threshold 0 --table_ratio 12 --table_size 35 --reordering_trigger="sa" \
  "$MODELS_PATH"/"$model".aag > $QUALITY_RESULTS_PATH/sylvan-solver-sa-"$model".txt
done

# measure quality of the manual reordering
for model in "${manu_models[@]}"
do
  ./sylvan-solver --threads 1 --nodes_threshold 0 --table_ratio 12 --table_size 35 --reordering_trigger="m" \
  "$MODELS_PATH"/"$model".aag > $QUALITY_RESULTS_PATH/sylvan-solver-m-"$model".txt
done

# measure quality of the automatic reordering
for model in "${auto_models[@]}"
do
  ./cudd-solver --reordering_trigger="a" \
  "$MODELS_PATH"/"$model".aag > $QUALITY_RESULTS_PATH/cudd-solver-a-"$model".txt
done

# measure quality of the semi-automatic reordering
for model in "${auto_models[@]}"
do
  ./cudd-solver --reordering_trigger="sa" \
  "$MODELS_PATH"/"$model".aag > $QUALITY_RESULTS_PATH/cudd-solver-sa-"$model".txt
done

# measure quality of the manual reordering
for model in "${manu_models[@]}"
do
  ./cudd-solver --reordering_trigger="m" \
  "$MODELS_PATH"/"$model".aag > $QUALITY_RESULTS_PATH/cudd-solver-m-"$model".txt
done

# measure runtime of the reordering
for model in "${auto_models[@]}"
do
   hyperfine -L solver "${separated_solvers_str}" -L model $MODELS_PATH/"$model".aag --warmup 1 './{solver} {model}' --export-csv $RESULTS_AUTO_RUNTIME/"$model".csv --export-markdown $RESULTS_AUTO/"$model".md
done



manu_solvers_str="${manu_solvers[*]}"
separated_manu_solvers_str=${manu_solvers_str// /,}

for solver in "${manu_solvers[@]}"
do
  chmod +x "$solver"
done

# measure quality of the reordering
for solver in "${manu_solvers[@]}"
do
  for model in "${manu_models[@]}"
  do
    ./"$solver" "$MODELS_PATH"/"$model".aag > $RESULTS_MANU_QUALITY/"$solver"-"$model".txt
  done
done

# measure runtime of the reordering
for model in "${manu_models[@]}"
do
   hyperfine -L solver "${separated_manu_solvers_str}" -L model $MODELS_PATH/"$model".aag './{solver} {model}' --runs 2 --export-csv $RESULTS_MANU_RUNTIME/"$model".csv
done

## MANUAL MANY REORDERING TESTS
declare -a manu_models=(
  "add2y"
  "add4y"
  "add6y"
  "add8y"
  "add10y"
  "add12y"
  "add14y"
  "mult_bool_matrix_2_3_2"
  "mult_bool_matrix_2_3_3"
  "mult_bool_matrix_2_3_4"
  "mult_bool_matrix_2_3_5"
  "mult_bool_matrix_2_3_6"
  "mult_bool_matrix_2_3_7"
  "mult_bool_matrix_2_3_8"
  "demo-v5_2_REAL"
  "demo-v3_2_REAL"
  "demo-v3_5_REAL"
  "demo-v4_5_REAL"
  "demo-v5_5_REAL"
  "demo-v6_5_REAL"
  "demo-v7_2_REAL"
)
declare -a manu_solvers=("cudd-sift-manu-many-solver" "cudd-group-manu-many-solver" "sylvan-manu-many-solver")

manu_solvers_str="${manu_solvers[*]}"
separated_manu_solvers_str=${manu_solvers_str// /,}

for solver in "${manu_solvers[@]}"
do
  chmod +x "$solver"
done

# measure quality of the reordering
for solver in "${manu_solvers[@]}"
do
  for model in "${manu_models[@]}"
  do
    ./"$solver" "$MODELS_PATH"/"$model".aag > $RESULTS_MANU_QUALITY/"$solver"-"$model".txt
  done
done

# measure runtime of the reordering
for model in "${manu_models[@]}"
do
   hyperfine -L solver "${separated_manu_solvers_str}" -L model $MODELS_PATH/"$model".aag './{solver} {model}' --runs 2 --export-csv $RESULTS_MANU_MANY_RUNTIME/"$model".csv
done