MODELS_PATH="./models"
RESULTS_PATH="./results/sylvan-regression"

QUALITY_RESULTS_PATH=$RESULTS_PATH"/quality"
RUNTIME_RESULTS_PATH=$RESULTS_PATH"/runtime"

echo ""
echo "Sylvan Regression Test [$(date)] START"

rm -r $QUALITY_RESULTS_PATH/*
rm -r $RUNTIME_RESULTS_PATH/*

chmod +x "sylvan-solver"
chmod +x "hyperfine"

declare -a tuning_models=(
  "add10y"
  "add12y"
  "mult_bool_matrix_2_3_3"
  "mult_bool_matrix_2_3_4"
)

echo "Sylvan Regression Test [$(date +%H:%M:%S)] testing quality effect of the max_growth parameter..."
declare -a iterations=(1 2 3)
declare -a workers=(4)

# measure measure quality effect of the tuning parameters
declare -a nodes_thresholds=(1)
for nt in "${nodes_thresholds[@]}"
do
  declare -a max_vars=(2000)
  for mv in "${max_vars[@]}"
  do
    declare -a max_swaps=(10000)
    for ms in "${max_swaps[@]}"
    do
      declare -a max_growths=(1.0 1.05 1.1 1.15 1.2 1.25 1.3 1.35 1.4)
      for mg in "${max_growths[@]}"
      do
        declare -a reordering_triggers=("m")
        for rt in "${reordering_triggers[@]}"
        do
          for n in "${workers[@]}"
          do
            for model in "${tuning_models[@]}"
            do
              for i in "${iterations[@]}"
              do
                ./sylvan-solver \
                  -n $n --nt $nt --tr 12 --ts 35 --mg $mg --mv $mv --ms $ms --rt=$rt \
                  "$MODELS_PATH"/"$model".aag > $QUALITY_RESULTS_PATH/"$n"-"$nt"-"$mg"-"$mv"-"$ms"-"$rt"-"$model"_max_growth-"$i".txt
              done
            done
          done
        done
      done
    done
  done
done

echo "Sylvan Regression Test [$(date +%H:%M:%S)] testing quality effect of the nodes_thresholds parameter..."

# measure measure quality effect of the tuning parameters
declare -a nodes_thresholds=(1 128 256 512 768 1024)
for nt in "${nodes_thresholds[@]}"
do
  declare -a max_vars=(2000)
  for mv in "${max_vars[@]}"
  do
    declare -a max_swaps=(10000)
    for ms in "${max_swaps[@]}"
    do
      declare -a max_growths=(1.2)
      for mg in "${max_growths[@]}"
      do
        declare -a reordering_triggers=("m")
        for rt in "${reordering_triggers[@]}"
        do
          for n in "${workers[@]}"
          do
            for model in "${tuning_models[@]}"
            do
              for i in "${iterations[@]}"
              do
                ./sylvan-solver \
                  -n $n --nt $nt --tr 12 --ts 35 --mg $mg --mv $mv --ms $ms --rt=$rt \
                  "$MODELS_PATH"/"$model".aag > $QUALITY_RESULTS_PATH/"$n"-"$nt"-"$mg"-"$mv"-"$ms"-"$rt"-"$model"_nodes_threshold-"$i".txt
              done
            done
          done
        done
      done
    done
  done
done

echo "Sylvan Regression Test [$(date +%H:%M:%S)] testing runtime effect of the number of workers..."

# measure measure runtime effect of the tuning number of workers
for model in "${tuning_models[@]}"
do
   ./hyperfine \
     -L model $MODELS_PATH/"$model".aag \
     -L workers 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16 \
     -L nodes_threshold 1 \
     -L max_var 2000 \
     -L max_swap 10000 \
     -L max_growth 1.2 \
     -L table_ratio 8 \
     -L table_size 30 \
     -L reordering_trigger "m" \
     --runs 3 \
     --warmup 1 \
     './sylvan-solver -n {workers} --nt {nodes_threshold} --tr {table_ratio} --ts {table_size} --mg {max_growth} --mv {max_var} --ms {max_swap} --rt={reordering_trigger} {model}' \
     --export-csv $RUNTIME_RESULTS_PATH/"$model"_workers.csv \
     > $RUNTIME_RESULTS_PATH/"$model"_workers.txt
done

echo "Sylvan Regression Test [$(date)] END"
echo ""