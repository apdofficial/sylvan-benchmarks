# Sylvan dynamic variable reordering benchmarks

## This repository contains the following benchmark usecases:
- [DPMC](https://github.com/allrtaken/DPMC): Solver that solves weighted model counting (WMC)
- [STORM](https://github.com/moves-rwth/storm): A Modern Probabilistic Model Checker
- [safety_synthesis](http://www.syntcomp.org/rules/): Safety Synthesis solver
- [hashmap_cmp](https://github.com/apdofficial/sylvan-benchmarks/tree/main/hashmap_cmp): Comparison of linear probing vs chaining
- [sylvancudd_cmp](https://github.com/apdofficial/sylvan-benchmarks/tree/main/sylvancudd_cmp): Comparison of [Sylvan](https://github.com/trolando/sylvan) and [CUDD](https://web.archive.org/web/20150215010013/http://vlsi.colorado.edu/~fabio/)
### Note
Currently DPMC implements additional logcounting function whcih is not at the moment part of Sylvan. Hence, in this repo we use Sylvan specifically from the following branch [feature/MSC-54](https://github.com/apdofficial/sylvan/tree/feature/MSC-54) which is provides the latest changes from my thesis as well as the logcounting.

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
 

