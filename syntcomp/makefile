cudd:
	time cabal run cudd-solver $(model) \
    --extra-lib-dirs=/opt/homebrew/Cellar/cudd/3.0.0/lib \
    --extra-lib-dirs=/Users/andrejpistek/Developer/MSc/sylvan/build/_deps/lace-build \
    --extra-lib-dirs=/Users/andrejpistek/Developer/MSc/sylvan/build/src \
    --extra-include-dirs=/opt/homebrew/Cellar/cudd/3.0.0/include \
    --extra-include-dirs=/Users/andrejpistek/Developer/MSc/sylvan/build/_deps/lace-src/src \
    --extra-include-dirs=/Users/andrejpistek/Developer/MSc/sylvan/build/_deps/lace-build \
    --extra-include-dirs=/Users/andrejpistek/Developer/MSc/sylvan/src

sylvan:
	time cabal run sylvan-solver $(model) \
    --extra-lib-dirs=/opt/homebrew/Cellar/cudd/3.0.0/lib \
    --extra-lib-dirs=/Users/andrejpistek/Developer/MSc/sylvan/build/_deps/lace-build \
    --extra-lib-dirs=/Users/andrejpistek/Developer/MSc/sylvan/build/src \
    --extra-include-dirs=/opt/homebrew/Cellar/cudd/3.0.0/include \
    --extra-include-dirs=/Users/andrejpistek/Developer/MSc/sylvan/build/_deps/lace-src/src \
    --extra-include-dirs=/Users/andrejpistek/Developer/MSc/sylvan/build/_deps/lace-build \
    --extra-include-dirs=/Users/andrejpistek/Developer/MSc/sylvan/src

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
