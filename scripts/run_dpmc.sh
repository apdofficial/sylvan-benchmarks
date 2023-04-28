cnf_file_path=$1

chmod +x build_dmc.sh
./build_dmc.sh
echo "Running DPMC benchmark"
# shellcheck disable=SC2217
cd "$DPMC_BUILD" &&  ./lg "./htd_main-1.2.0 -s 1234567 --print-progress --strategy challenge --opt width --iterations 0 --preprocessing full --patience 20" < "$cnf_file_path" | ./dmc --pw=3 --cf "$cnf_file_path" --dp=s --lc=1 --wc=0 --vs=2 --dv=5 --dy=1 --mm=1000 --jp=f --sa=0 --aa=1 --er=0 --tc=7 --tr=8 --ir=5
