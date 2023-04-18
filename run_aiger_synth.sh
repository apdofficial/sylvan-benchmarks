agg_file_path=$1

# precondition: the build directory exists and contains the executable aiger_synt + the input data
run_safety_synthesis_benchmark()
{
    echo "Running safety synthesis benchmark"
    cd ./build/safety_synthesis || exit
    ./aiger_synt -w 4 "../../$agg_file_path"
    cd ../../ || exit
}

run_safety_synthesis_benchmark