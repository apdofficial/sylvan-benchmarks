# precondition: the build directory exists and contains the executables storm + the input data
run_storm_benchmark()
{
#    input_file_path=$1
    echo "Running STORM benchmark"
    cd ./build/storm || exit
    # run benchmark
    cd ../../ || exit
}
