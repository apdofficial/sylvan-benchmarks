# Sylvan dynamic variable reordering benchmarks

## This repository contains the following benchmark usecases:
- [DPMC](https://github.com/allrtaken/DPMC): Solver that solves weighted model counting (WMC)
- [safety_synthesis](http://www.syntcomp.org/rules/): Safety Synthesis solver
- [cmp_hashmap](https://github.com/apdofficial/sylvan-benchmarks/tree/main/hashmap_cmp): Comparison of linear probing vs chaining
- [cmp_sylvancudd](https://github.com/apdofficial/sylvan-benchmarks/tree/main/sylvancudd_cmp): Comparison of [Sylvan](https://github.com/trolando/sylvan) and [CUDD](https://web.archive.org/web/20150215010013/http://vlsi.colorado.edu/~fabio/)
- [syntcomp](https://github.com/apdofficial/sylvan-benchmarks/tree/main/syntcomp): A simple BDD based safety game solver for the Reactive Synthesis Competition.
# Usage

1.  The repository contains a Docker file using which you can build a docker image **sylvanbenchmarks**:
    ```shell
    docker build --pull --rm -f "Dockerfile" -t sylvanbenchmarks:latest "." 
    ```
    The dokcer file creates new directory **build** (overrriden in case it already exists) and compiles all the executables into it together with the  necesarry input data.
2. Run your image as a interactive container using:
    ```shell
    docker run -i -t sylvanbenchmarks:latest
    ```
3. inside of the image run the following shell script with all the benchmarks (work directory is set to the project working directory):
    ```shell
    cd $SCRIPTS && ./run_all.sh
    ```
    or run a specific benchmark, eg.:
    ```shell
    cd $SCRIPTS && ./build_dmc.sh
    ```
   ```shell
    cd $SCRIPTS && ./test_sylvan_dpmc.sh "$DMPC_TESTS/weighted/mcc21__wff.3.75.315.cnf"
    ```
   ```shell
    cd $SCRIPTS && ./test_cudd_dpmc.sh "$DMPC_TESTS/weighted/mcc21__wff.3.75.315.cnf"
    ```
    ```shell
    cd $SCRIPTS && ./run_safety_synth.sh "$SAFETY_SYNT_MODELS/driver/driver_d10y.aag"
    ```
