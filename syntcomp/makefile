SYLVAN_INC_PATH=/Users/andrejpistek/Developer/MSc/sylvan-benchmarks/external/sylvan/src
# SYLVAN_LIB_PATH=/Users/andrejpistek/Developer/MSc/sylvan-benchmarks/cmake-build-debug-clang/external/sylvan/src
SYLVAN_LIB_PATH=/Users/andrejpistek/Developer/MSc/sylvan-benchmarks/cmake-build-release-clang/external/sylvan/src

LACE_INC_PATH=/Users/andrejpistek/Developer/MSc/sylvan-benchmarks/cmake-build-release-clang/_deps/lace-src/src
LACE_LIB_PATH=/Users/andrejpistek/Developer/MSc/sylvan-benchmarks/cmake-build-release-clang/_deps/lace-build

CUDD_INC_PATH=/Users/andrejpistek/Developer/MSc/sylvan-benchmarks/external/cudd/cudd
CUDD_LIB_PATH=/Users/andrejpistek/Developer/MSc/sylvan-benchmarks/cmake-build-debug-clang/external/cudd/cudd

CUDD_INC_PATH=/opt/homebrew/Cellar/cudd/3.0.0/include
CUDD_LIB_PATH=/opt/homebrew/Cellar/cudd/3.0.0/lib

cudd:
	cabal run cudd-solver $(model) \
    --extra-lib-dirs=${CUDD_LIB_PATH} \
    --extra-lib-dirs=${LACE_LIB_PATH} \
    --extra-lib-dirs=${SYLVAN_LIB_PATH} \
    --extra-include-dirs=${CUDD_INC_PATH} \
    --extra-include-dirs=${LACE_INC_PATH} \
    --extra-include-dirs=${SYLVAN_INC_PATH}

sylvan:
	cabal run sylvan-solver $(model) \
    --extra-lib-dirs=${CUDD_LIB_PATH} \
    --extra-lib-dirs=${LACE_LIB_PATH} \
    --extra-lib-dirs=${SYLVAN_LIB_PATH} \
    --extra-include-dirs=${CUDD_INC_PATH} \
    --extra-include-dirs=${LACE_INC_PATH} \
    --extra-include-dirs=${SYLVAN_INC_PATH}

clean:
	@echo "cleaning..." 
	cabal clean
	@echo "cleaning done"

help:
	@echo " " 
	@echo "BDD Benchmark to compare dynamic variable reorderings using Adam Wlaker's BDD safety game solver." 
	@echo "--------------" 
	@echo "A collection of benchmarks to compare dynamic variable reordering between Sylvan and CUDD" 
	@echo "Please update the --extra-lib-dirs as well as the extra-include-dirs paths based on where you isntalled cudd, lace and sylvan libs."
