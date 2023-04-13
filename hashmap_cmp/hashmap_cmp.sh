run_the_benchmark()
{
  cd ./hashmap_cmp || exit
  chmod +x hashmap_cmp
  ./hashmap_cmp
  rm hashmap_cmp
  cd ../
}

compile_the_target()
{
  cmake --build . --target hashmap_cmp
}

# we expect to be in the current directory
cd ../
rm -f build
mkdir build
cd build || exit

cmake .. -DCMAKE_BUILD_TYPE=Debug -DDSYLVAN_USE_LINEAR_PROBING=ON -DSYLVAN_LIMIT_PROBE_SEQUENCE=ON
compile_the_target
echo "Running hashmap_cmp with linear probing + limit the probe sequence"
run_the_benchmark

cmake .. -DCMAKE_BUILD_TYPE=Debug -DDSYLVAN_USE_LINEAR_PROBING=OFF -DSYLVAN_LIMIT_PROBE_SEQUENCE=OFF
compile_the_target
echo "Running hashmap_cmp with linear probing + dont limit the probe sequence"
run_the_benchmark

cmake .. -DCMAKE_BUILD_TYPE=Debug
compile_the_target
echo "Running hashmap_cmp with chaining"
run_the_benchmark

echo "Plotting the results"
cd ../
cd hashmap_cmp || exit
python3 hashmap_cmp.py
rm -f build