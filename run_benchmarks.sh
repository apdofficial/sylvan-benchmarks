# precondition: the build directory exists and contains the executables lg, htd_main-1.2.0 and dmc + the input data
run_dpmc_benchmark()
{
    cnf_file_path=$1
    cd ./build/dpmc || exit 
   ./lg "./htd_main-1.2.0 -s 1234567 --print-progress --strategy challenge --opt width --iterations 0 --preprocessing full --patience 20" < $cnf_file_path | ./dmc --pw=3 --cf $cnf_file_path --dp=s --lc=1 --wc=0 --vs=2 --dv=5 --dy=0 --mm=6500 --jp=f --sa=0 --aa=1 --er=0 --tc=7 --tr=3 --ir=5
    cd ../../ || exit 
}

# precondition: the build directory exists and contains the executables storm + the input data
run_storm_benchmark()
{
    input_file_path=$1
    cd ./build/storm || exit 
    # run benchmark
    cd ../../ || exit 
}

# precondition: the build directory exists and contains the executables safety_synthesis + the input data
run_safety_synthesis_benchmark()
{
    input_file_path=$1
    cd ./build/safety_synthesis || exit 
    # run benchmark
    cd ../../ || exit 
}

echo "Running DPMC benchmark"
run_dpmc_benchmark "./mcc21__wff.3.75.315.cnf"

# echo "Running STORM benchmark"
# run_storm_benchmark 

# echo "Running safety synthesis benchmark"
# run_safety_synthesis_benchmark 