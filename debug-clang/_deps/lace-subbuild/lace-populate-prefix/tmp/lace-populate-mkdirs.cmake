# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/Users/andrejpistek/Developer/MSc/sylvan-benchmarks/debug-clang/_deps/lace-src"
  "/Users/andrejpistek/Developer/MSc/sylvan-benchmarks/debug-clang/_deps/lace-build"
  "/Users/andrejpistek/Developer/MSc/sylvan-benchmarks/debug-clang/_deps/lace-subbuild/lace-populate-prefix"
  "/Users/andrejpistek/Developer/MSc/sylvan-benchmarks/debug-clang/_deps/lace-subbuild/lace-populate-prefix/tmp"
  "/Users/andrejpistek/Developer/MSc/sylvan-benchmarks/debug-clang/_deps/lace-subbuild/lace-populate-prefix/src/lace-populate-stamp"
  "/Users/andrejpistek/Developer/MSc/sylvan-benchmarks/debug-clang/_deps/lace-subbuild/lace-populate-prefix/src"
  "/Users/andrejpistek/Developer/MSc/sylvan-benchmarks/debug-clang/_deps/lace-subbuild/lace-populate-prefix/src/lace-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/andrejpistek/Developer/MSc/sylvan-benchmarks/debug-clang/_deps/lace-subbuild/lace-populate-prefix/src/lace-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/andrejpistek/Developer/MSc/sylvan-benchmarks/debug-clang/_deps/lace-subbuild/lace-populate-prefix/src/lace-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
