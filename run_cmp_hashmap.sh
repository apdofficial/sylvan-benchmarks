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

rm -f build
mkdir build
cd build || exit

cmake .. -DCMAKE_BUILD_TYPE=Debug -DSYLVAN_USE_LINEAR_PROBING=ON
compile_cmp_hashmap
echo "Running cmp_hashmap with linear probing"
run_cmp_hashmap

cmake .. -DCMAKE_BUILD_TYPE=Debug -DSYLVAN_USE_LINEAR_PROBING=OFF
compile_cmp_hashmap
echo "Running cmp_hashmap with chaining"
run_cmp_hashmap

echo "Plotting the results"
cd ../
cd cmp_hashmap || exit
python3 cmp_hashmap.py
#rm -f build