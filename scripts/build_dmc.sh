# prepare the dmc executable
rm -r "$ADDMC"/libraries/sylvan/build
cd "$ADDMC"/ && make sylvan -j 6
cd "$ADDMC"/ && make dmc -j 6
cp "$ADDMC"/dmc "$DPMC_BUILD"

# prepare the lg executable
cd "$LG"/ && make -j 6
cp "$LG"/build/lg "$DPMC_BUILD"

# prepare the htd_main-1.2.0 executable
cd "$HTD_SOLVER" && mkdir build
cd "$HTD_SOLVER"/build && cmake .. && make -j 6
cp "$HTD_SOLVER"/build/bin/htd_main-1.2.0 "$DPMC_BUILD"

cp "$DMPC_WEIGHTED_TESTS"/mcc21__wff.3.75.315.cnf "$DPMC_BUILD"