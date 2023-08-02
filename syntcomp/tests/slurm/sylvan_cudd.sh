MODELS_PATH="./models"
RESULTS_PATH="./results/sylvan-cudd"

QUALITY_RESULTS_PATH=$RESULTS_PATH"/quality"
RUNTIME_RESULTS_PATH=$RESULTS_PATH"/runtime"

echo ""
echo "Sylvan vs CUDD Test [$(date)] START"

rm -r $QUALITY_RESULTS_PATH/*
rm -r $RUNTIME_RESULTS_PATH/*

chmod +x "sylvan-solver"
chmod +x "cudd-solver"
chmod +x "hyperfine"

echo "Sylvan vs CUDD Test [$(date +%H:%M:%S)] testing quality of the Sylvan & CUDD manual reordering..."

declare -a manu_quality_models=(
  "add10y"
  "add12y"
  "add14y"
  "add16y"
  "mult_bool_matrix_2_3_5"
  "mult_bool_matrix_2_3_6"
  "mult_bool_matrix_2_3_7"
  "mult_bool_matrix_2_3_8"
)

for model in "${manu_quality_models[@]}"
do
  n=16
  nt=1
  mg=1.2
  mv=200
  ms=10000
  rt="m"
  ./sylvan-solver \
    -n $n --nt $nt --tr 12 --ts 36 --mg $mg --mv $mv --ms $ms --rt=$rt \
    "$MODELS_PATH"/"$model".aag > $QUALITY_RESULTS_PATH/sylvan-"$n"-"$nt"-"$mg"-"$mv"-"$ms"-"$rt"-"$model".txt

  ./cudd-solver \
    --rt=$rt --h="group"\
    "$MODELS_PATH"/"$model".aag > $QUALITY_RESULTS_PATH/cudd-"$rt"-group-"$model".txt

  ./cudd-solver \
    --rt=$rt --h="sift"\
    "$MODELS_PATH"/"$model".aag > $QUALITY_RESULTS_PATH/cudd-"$rt"-sift-"$model".txt
done

echo "Sylvan vs CUDD Test [$(date +%H:%M:%S)] testing quality of the Sylvan & CUDD semi-automatic reordering..."

declare -a auto_quality_models=(
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
)
for model in "${auto_quality_models[@]}"
do
  n=8
  nt=1
  mg=1.2
  mv=200
  ms=10000
  rt="sa"
  ./sylvan-solver \
    -n $n --nt $nt --tr 13 --ts 33 --mg $mg --mv $mv --ms $ms --rt=$rt \
    "$MODELS_PATH"/"$model".aag > $QUALITY_RESULTS_PATH/sylvan-"$n"-"$nt"-"$mg"-"$mv"-"$ms"-"$rt"-"$model".txt

  ./cudd-solver \
    --rt=$rt --h="group"\
    "$MODELS_PATH"/"$model".aag > $QUALITY_RESULTS_PATH/cudd-"$rt"-group-"$model".txt

  ./cudd-solver \
    --rt=$rt --h="sift"\
    "$MODELS_PATH"/"$model".aag > $QUALITY_RESULTS_PATH/cudd-"$rt"-sift-"$model".txt
done

echo "Sylvan vs CUDD Test [$(date +%H:%M:%S)] testing quality of the CUDD automatic reordering..."


for model in "${auto_quality_models[@]}"
do
  rt="a"
  ./cudd-solver \
    --rt=$rt --h="group" \
    "$MODELS_PATH"/"$model".aag > $QUALITY_RESULTS_PATH/cudd-"$rt"-group-"$model".txt
  ./cudd-solver \
    --rt=$rt --h="sift"\
    "$MODELS_PATH"/"$model".aag > $QUALITY_RESULTS_PATH/cudd-"$rt"-sift-"$model".txt
done

echo "Sylvan vs CUDD Test [$(date +%H:%M:%S)] testing runtime of the Sylvan & CUDD manual reordering..."

declare -a manu_runtime_models=(
  "add10y"
  "add12y"
  "add14y"
  "add16y"
  "mult_bool_matrix_2_3_5"
  "mult_bool_matrix_2_3_6"
  "mult_bool_matrix_2_3_7"
  "mult_bool_matrix_2_3_8"
)
for model in "${manu_runtime_models[@]}"
do
   ./hyperfine \
     -L reordering-trigger "m" \
     -L solver "sylvan-solver -n 8 --nt 1 --tr 12 --ts 36 --mg 1.2 --mv 2000 --ms 100000","sylvan-solver -n 16 --nt 1 --tr 12 --ts 36 --mg 1.2 --mv 2000 --ms 100000","cudd-solver --h="sift"","cudd-solver --h="group"" \
     -L model $MODELS_PATH/"$model".aag \
     --runs 1 \
     --warmup 1 \
     './{solver} --rt={reordering-trigger} {model}' \
     --export-csv $RUNTIME_RESULTS_PATH/m-"$model".csv \
     > $RUNTIME_RESULTS_PATH/m-"$model".txt
done

echo "Sylvan vs CUDD Test [$(date +%H:%M:%S)] testing runtime of the Sylvan & CUDD semi-automatic reordering..."

declare -a auto_runtime_models=(
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
)

for model in "${auto_runtime_models[@]}"
do
   ./hyperfine \
     -L reordering-trigger "sa" \
     -L solver "sylvan-solver -n 8 --nt 1 --tr 14 --ts 33 --mg 1.2 --mv 200 --ms 10000","cudd-solver --h="sift"","cudd-solver --h="group"" \
     -L model $MODELS_PATH/"$model".aag \
     --runs 10 \
     --warmup 1 \
     './{solver} --rt={reordering-trigger} {model}' \
     --export-csv $RUNTIME_RESULTS_PATH/sa-"$model".csv \
     > $RUNTIME_RESULTS_PATH/sa-"$model".txt
done

echo "Sylvan vs CUDD Test [$(date +%H:%M:%S)] testing runtime of the CUDD automatic reordering..."

for model in "${auto_runtime_models[@]}"
do
   ./hyperfine \
     -L heuristic "sift","group" \
     -L reordering-trigger "a" \
     -L solver "cudd-solver " \
     -L model $MODELS_PATH/"$model".aag \
     --runs 10 \
     --warmup 1 \
     './{solver} --rt={reordering-trigger} --h={heuristic} {model}' \
     --export-csv $RUNTIME_RESULTS_PATH/a-"$model".csv \
     > $RUNTIME_RESULTS_PATH/a-"$model".txt
done

echo "Sylvan vs CUDD Test [$(date)] END"
echo ""
