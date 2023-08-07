MODELS_PATH="./models"
RESULTS_PATH="./results/sylvan-regression"

QUALITY_RESULTS_PATH=$RESULTS_PATH"/quality"
RUNTIME_RESULTS_PATH=$RESULTS_PATH"/runtime"

rm -r $QUALITY_RESULTS_PATH/*
rm -r $RUNTIME_RESULTS_PATH/*

echo ""
echo "Sylvan Regression Test [$(date)] START"

chmod +x "sylvan-solver"
chmod +x "hyperfine"

declare -a tuning_models=(
  "add10y"
  "add12y"
  "mult_bool_matrix_2_3_3"
  "mult_bool_matrix_2_3_4"
)

declare -a iterations=(1 2 3 4 5)

#echo "Sylvan Regression Test [$(date +%H:%M:%S)] testing effect of the max_growth parameter..."

declare -a workers=(4)
declare -a max_growths=(1.0 1.1 1.2 1.3)
declare -a max_swaps=(10000)
declare -a max_vars=(2000)
declare -a nodes_thresholds=(1)
declare -a reordering_triggers=("m")

#for nt in "${nodes_thresholds[@]}"
#do
#  for mv in "${max_vars[@]}"
#  do
#    for ms in "${max_swaps[@]}"
#    do
#      for mg in "${max_growths[@]}"
#      do
#        for rt in "${reordering_triggers[@]}"
#        do
#          for n in "${workers[@]}"
#          do
#            for model in "${tuning_models[@]}"
#            do
#              for i in "${iterations[@]}"
#              do
#                ./sylvan-solver \
#                  -n $n --nt $nt --tr 8 --ts 30 --mg $mg --mv $mv --ms $ms --rt=$rt \
#                  "$MODELS_PATH"/"$model".aag > $QUALITY_RESULTS_PATH/"$n"-"$nt"-"$mg"-"$mv"-"$ms"-"$rt"-"$model"_max_growth-"$i".txt
#              done
#            done
#          done
#        done
#      done
#    done
#  done
#done

#echo "Sylvan Regression Test [$(date +%H:%M:%S)] testing effect of the nodes_thresholds parameter..."

declare -a max_growths=(1.2)
declare -a max_swaps=(10000)
declare -a max_vars=(2000)
declare -a nodes_thresholds=(1 128 256 512 768 1024)
declare -a reordering_triggers=("m")

#for nt in "${nodes_thresholds[@]}"
#do
#  for mv in "${max_vars[@]}"
#  do
#    for ms in "${max_swaps[@]}"
#    do
#      for mg in "${max_growths[@]}"
#      do
#        for rt in "${reordering_triggers[@]}"
#        do
#          for n in "${workers[@]}"
#          do
#            for model in "${tuning_models[@]}"
#            do
#              for i in "${iterations[@]}"
#              do
#                ./sylvan-solver \
#                  -n $n --nt $nt --tr 8 --ts 30 --mg $mg --mv $mv --ms $ms --rt=$rt \
#                  "$MODELS_PATH"/"$model".aag > $QUALITY_RESULTS_PATH/"$n"-"$nt"-"$mg"-"$mv"-"$ms"-"$rt"-"$model"_nodes_threshold-"$i".txt
#              done
#            done
#          done
#        done
#      done
#    done
#  done
#done

echo "Sylvan Regression Test [$(date +%H:%M:%S)] testing effect of the workers parameter..."

declare -a tuning_models=(
  "add12y"
  "add14y"
  "mult_bool_matrix_2_3_4"
  "mult_bool_matrix_2_3_5"
)
declare -a iterations=(1 2 3)
declare -a max_growths=(1.2)
declare -a max_swaps=(10000)
declare -a max_vars=(2000)
declare -a nodes_thresholds=(1)
declare -a nodes_thresholds=(1)
declare -a reordering_triggers=("m")
declare -a workers=(1 2 4 6 8 10 12 14 16)

for nt in "${nodes_thresholds[@]}"
do
  for mv in "${max_vars[@]}"
  do
    for ms in "${max_swaps[@]}"
    do
      for mg in "${max_growths[@]}"
      do
        for rt in "${reordering_triggers[@]}"
        do
          for n in "${workers[@]}"
          do
            for model in "${tuning_models[@]}"
            do
              for i in "${iterations[@]}"
              do
                ./sylvan-solver \
                  -n $n --nt $nt --tr 2 --ts 32 --mg $mg --mv $mv --ms $ms --rt=$rt \
                  "$MODELS_PATH"/"$model".aag > $QUALITY_RESULTS_PATH/"$n"-"$nt"-"$mg"-"$mv"-"$ms"-"$rt"-"$model"_workers-"$i".txt
              done
            done
          done
        done
      done
    done
  done
done

echo "Sylvan Regression Test [$(date)] END"
echo ""