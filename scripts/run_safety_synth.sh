agg_file_path=$1

# precondition: the build directory exists and contains the executable aiger_synt + the input data

chmod +x build_safety_synth.sh
./build_safety_synth.sh
echo "Running safety synthesis benchmark"
cd "$SAFETY_SYNT_BUILD" && ./aiger_synt -w 4 "$agg_file_path"