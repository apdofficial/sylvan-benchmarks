MODELS_PATH="./models"
RESULTS_PATH="./results/sylvan-regression"

QUALITY_RESULTS_PATH=$RESULTS_PATH"/quality"
RUNTIME_RESULTS_PATH=$RESULTS_PATH"/runtime"

rm -r $QUALITY_RESULTS_PATH/*
rm -r $RUNTIME_RESULTS_PATH/*

chmod +x "sylvan-solver"
chmod +x "hyperfine"

echo "Sylvan Regression Test [$(date)] start"

declare -a tuning_quality_models=(
  "add10y"
  "add14y"
  "mult_bool_matrix_2_3_2"
  "mult_bool_matrix_2_3_4"
)

echo "Sylvan Regression Test [$(date)] testing quality effect of the tuning parameters..."

# measure measure quality effect of the tuning parameters
declare -a nodes_thresholds=(1 128 256)
for nt in "${nodes_thresholds[@]}"
do
  declare -a max_vars=(50 100 200)
  for mv in "${max_vars[@]}"
  do
    declare -a max_swaps=(1000 5000 10000)
    for ms in "${max_swaps[@]}"
    do
      declare -a max_growths=(1.0 1.2 1.4)
      for mg in "${max_growths[@]}"
      do
        declare -a reordering_triggers=("sa" "m")
        for rt in "${reordering_triggers[@]}"
        do
          declare -a workers=(1 4 8 16 32)
          for n in "${workers[@]}"
          do

            for model in "${tuning_quality_models[@]}"
            do
              ./sylvan-solver \
                -n $n --nt $nt --tr 12 --ts 35 --mg $mg --mv $mv --ms $ms --rt=$rt \
                "$MODELS_PATH"/"$model".aag > $QUALITY_RESULTS_PATH/"$n"-"$nt"-"$mg"-"$mv"-"$ms"-"$rt"-"$model".txt
            done

          done
        done
      done
    done
  done
done

echo "Sylvan Regression Test [$(date)] testing runtime effect of the tuning parameters..."

declare -a tuning_runtime_models=(
  "add10y"
  "add20y"
  "mult_bool_matrix_2_3_4"
  "mult_bool_matrix_2_3_7"
)

# measure measure runtime effect of the tuning parameters
for model in "${tuning_runtime_models[@]}"
do
   hyperfine \
     -L model $MODELS_PATH/"$model".aag \
     -L workers 1,2,3,4,5,6,7,8,9,10,11,12,14,16,18,24,32 \
     -L nodes_threshold 1,128,256 \
     -L max_var 50,100,200 \
     -L max_swap 1000,5000,10000 \
     -L max_growth 1.0,1.2,1.4 \
     -L table_ratio 13 \
     -L table_size 35 \
     -L reordering_trigger "sa" \
     --warmup 1 \
     './sylvan-solver -n {workers} --nt {nodes_threshold} --tr {table_ratio} --ts {table_size} --mg {max_growth} --mv {max_var} --ms {max_swap} --rt={reordering_trigger} {model}' \
     --export-csv $RUNTIME_RESULTS_PATH/"$model".csv \
     > $RUNTIME_RESULTS_PATH/"$model".txt
done

echo "Sylvan Regression Test [$(date)] end"
