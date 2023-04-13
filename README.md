# Sylvan dynamic variable reordering benchmarks

## This repository contains the following benchmark usecases:
- [DPMC](https://github.com/allrtaken/DPMC): Solver that solves weighted model counting (WMC)
- [STORM](https://github.com/moves-rwth/storm): A Modern Probabilistic Model Checker
- [safety_synthesis](http://www.syntcomp.org/rules/): Safety Synthesis solver
- [hashmap_cmp](https://github.com/apdofficial/sylvan-benchmarks/tree/main/hashmap_cmp): Comparison of linear probing vs chaining
- [sylvancudd_cmp](https://github.com/apdofficial/sylvan-benchmarks/tree/main/sylvancudd_cmp): Comparison of Sylvan and CUDD
# Usage

1.  The repository contains a Docker file which you can build using:
    ```shell
    docker build --pull --rm -f "Dockerfile" -t sylvanbenchmarks:latest "." 
    ```
    The dokcer file creates new directory **build** (overrriden in case it already exists) and compiles all the executables into it together with the  necesarry input data.
2. Run your image as a interactive container using:
    ```shell
    docker run -i -t sylvanbenchmarks:latest
    ```
3. inside of the image run the following shell script with all the benchmarks (work directory is set to the project workign directory):
    ```shell
    ./run_benchmarks.sh
    ```
 

