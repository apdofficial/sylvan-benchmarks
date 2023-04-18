run_cmp_hashmap()
{
  cd ./cmp_hashmap || exit
  chmod +x cmp_hashmap
  ./cmp_hashmap
  rm cmp_hashmap
  cd ../
}

compile_cmp_hashmap()
{
  cmake --build . --target cmp_hashmap
}

# we expect to be in the current directory
cd ../
rm -f build
mkdir build
cd build || exit

cmake .. -DCMAKE_BUILD_TYPE=Debug -DSYLVAN_USE_LINEAR_PROBING=ON -DSYLVAN_LIMIT_PROBE_SEQUENCE=ON
compile_cmp_hashmap
echo "Running cmp_hashmap with linear probing + limit the probe sequence"
run_cmp_hashmap

cmake .. -DCMAKE_BUILD_TYPE=Debug -DSYLVAN_USE_LINEAR_PROBING=ON -DSYLVAN_LIMIT_PROBE_SEQUENCE=OFF
compile_cmp_hashmap
echo "Running cmp_hashmap with linear probing + dont limit the probe sequence"
run_cmp_hashmap

cmake .. -DCMAKE_BUILD_TYPE=Debug
compile_cmp_hashmap
echo "Running cmp_hashmap with chaining"
run_cmp_hashmap

echo "Plotting the results"
cd ../
cd cmp_hashmap || exit
python3 cmp_hashmap.py
rm -f build