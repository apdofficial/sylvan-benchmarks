cnf_file_path=$1

# shellcheck disable=SC2217
cd "$DPMC_BUILD" &&  ./lg "./htd_main-1.2.0 -s 1234567 --print-progress --strategy challenge --opt width --iterations 0 --preprocessing full --patience 20" < "$cnf_file_path" |
./dmc --pw=3 --cf "$cnf_file_path" --dp=c --lc=1 --wc=0 --vs=2 --dv=5 --dy=0 --mm=6500 --jp=f --sa=0 --aa=1 --er=0
