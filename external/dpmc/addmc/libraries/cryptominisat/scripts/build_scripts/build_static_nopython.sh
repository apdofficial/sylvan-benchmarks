#!/bin/bash

set -e

rm -rf cm* CM* lib* cryptomini* Testing* tests* pycryptosat include tests
cmake -DENABLE_PYTHON_INTERFACE=OFF -DREQUIRE_M4RI=ON -DSTATICCOMPILE=ON ..
make -j6 VERBOSE=1
strip cryptominisat5
