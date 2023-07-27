MODELS_PATH="/Users/andrejpistek/Developer/MSc/sylvan-benchmarks/safety_synthesis/models"
RESULTS_PATH="./results"

RESULTS_AUTO=$RESULTS_PATH/"auto"
RESULTS_MANU=$RESULTS_PATH/"manu"

rm -r $RESULTS_AUTO/*
rm -r $RESULTS_MANU/*

ADD=$MODELS_PATH/"toy_examples"
MUL=$MODELS_PATH/"mult_matrix"

# AUTOMATIC REORDERING TESTS
declare -a auto_add_models=("add10y" "add12y" "add14y" "add16y" "add18y" "add20y" "add24y" "add28y")
declare -a auto_mult_models=("mult_bool_matrix_2_3_4" "mult_bool_matrix_2_3_5" "mult_bool_matrix_2_3_6")
declare -a auto_solvers=("cudd-sift-auto-solver" "cudd-group-auto-solver" "sylvan-auto-solver")

auto_solvers_str="${auto_solvers[*]}"
separated_auto_solvers_str=${auto_solvers_str// /,}

for solver in "${auto_solvers[@]}"
do
  chmod +x "$solver"
done

# measure quality of the reordering
for solver in "${auto_solvers[@]}"
do
  for model in "${auto_add_models[@]}"
  do
    ./"$solver" "$ADD"/"$model".aag > $RESULTS_AUTO/"$solver"_"$model".txt
  done
  for model in "${auto_mult_models[@]}"
  do
    ./"$solver" "$MUL"/"$model".aag > $RESULTS_AUTO/"$solver"_"$model".txt
  done
done

# measure runtime of the reordering
for model in "${auto_add_models[@]}"
do
   hyperfine -L solver "${separated_auto_solvers_str}" -L model $ADD/"$model".aag --warmup 1 './{solver} {model}' --export-csv $RESULTS_AUTO/"$model".csv --export-markdown $RESULTS_AUTO/"$model".md
done
for model in "${auto_mult_models[@]}"
do
   hyperfine -L solver "${separated_auto_solvers_str}" -L model $MUL/"$model".aag --warmup 1 './{solver} {model}' --export-csv $RESULTS_AUTO/"$model".csv --export-markdown $RESULTS_AUTO/"$model".md
done

# MANUAL REORDERING TESTS
declare -a manu_add_models=("add10y" "add12y" "add14y")
declare -a manu_mult_models=("mult_bool_matrix_2_3_4" "mult_bool_matrix_2_3_5" "mult_bool_matrix_2_3_6")
declare -a manu_solvers=("cudd-sift-manu-solver" "cudd-group-manu-solver" "sylvan-manu-solver")

manu_solvers_str="${manu_solvers[*]}"
separated_manu_solvers_str=${manu_solvers_str// /,}

for solver in "${manu_solvers[@]}"
do
  chmod +x "$solver"
done

# measure quality of the reordering
for solver in "${manu_solvers[@]}"
do
  for model in "${manu_add_models[@]}"
  do
    ./"$solver" "$ADD"/"$model".aag > $RESULTS_MANU/"$solver"_"$model".txt
  done
  for model in "${manu_mult_models[@]}"
  do
    ./"$solver" "$MUL"/"$model".aag > $RESULTS_MANU/"$solver"_"$model".txt
  done
done

# measure runtime of the reordering
for model in "${manu_add_models[@]}"
do
   hyperfine -L solver "${separated_manu_solvers_str}" -L model $ADD/"$model".aag './{solver} {model}' --runs 2 --export-csv $RESULTS_MANU/"$model".csv --export-markdown $RESULTS_MANU/"$model".md
done
for model in "${manu_mult_models[@]}"
do
   hyperfine -L solver "${separated_manu_solvers_str}" -L model $MUL/"$model".aag  './{solver} {model}' --runs 2 --export-csv $RESULTS_MANU/"$model".csv --export-markdown $RESULTS_MANU/"$model".md
done