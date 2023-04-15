#!/bin/bash

set -e

rm -rf cm* CM* lib* cryptomini* Testing* tests* pycryptosat include tests cusp* scalmc*
cmake -DENABLE_PYTHON_INTERFACE=ON -DENABLE_TESTING=ON -DNOZLIB=ON ..
make -j6
make test
