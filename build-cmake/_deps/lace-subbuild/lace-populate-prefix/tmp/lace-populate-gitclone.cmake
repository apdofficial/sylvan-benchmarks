
if(NOT "/tmp/sylvan-benchmarks/build-cmake/_deps/lace-subbuild/lace-populate-prefix/src/lace-populate-stamp/lace-populate-gitinfo.txt" IS_NEWER_THAN "/tmp/sylvan-benchmarks/build-cmake/_deps/lace-subbuild/lace-populate-prefix/src/lace-populate-stamp/lace-populate-gitclone-lastrun.txt")
  message(STATUS "Avoiding repeated git clone, stamp file is up to date: '/tmp/sylvan-benchmarks/build-cmake/_deps/lace-subbuild/lace-populate-prefix/src/lace-populate-stamp/lace-populate-gitclone-lastrun.txt'")
  return()
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND} -E remove_directory "/tmp/sylvan-benchmarks/build-cmake/_deps/lace-src"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to remove directory: '/tmp/sylvan-benchmarks/build-cmake/_deps/lace-src'")
endif()

# try the clone 3 times in case there is an odd git clone issue
set(error_code 1)
set(number_of_tries 0)
while(error_code AND number_of_tries LESS 3)
  execute_process(
    COMMAND "/usr/bin/git"  clone --no-checkout "https://github.com/trolando/lace.git" "lace-src"
    WORKING_DIRECTORY "/tmp/sylvan-benchmarks/build-cmake/_deps"
    RESULT_VARIABLE error_code
    )
  math(EXPR number_of_tries "${number_of_tries} + 1")
endwhile()
if(number_of_tries GREATER 1)
  message(STATUS "Had to git clone more than once:
          ${number_of_tries} times.")
endif()
if(error_code)
  message(FATAL_ERROR "Failed to clone repository: 'https://github.com/trolando/lace.git'")
endif()

execute_process(
  COMMAND "/usr/bin/git"  checkout v1.3.1 --
  WORKING_DIRECTORY "/tmp/sylvan-benchmarks/build-cmake/_deps/lace-src"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to checkout tag: 'v1.3.1'")
endif()

set(init_submodules TRUE)
if(init_submodules)
  execute_process(
    COMMAND "/usr/bin/git"  submodule update --recursive --init 
    WORKING_DIRECTORY "/tmp/sylvan-benchmarks/build-cmake/_deps/lace-src"
    RESULT_VARIABLE error_code
    )
endif()
if(error_code)
  message(FATAL_ERROR "Failed to update submodules in: '/tmp/sylvan-benchmarks/build-cmake/_deps/lace-src'")
endif()

# Complete success, update the script-last-run stamp file:
#
execute_process(
  COMMAND ${CMAKE_COMMAND} -E copy
    "/tmp/sylvan-benchmarks/build-cmake/_deps/lace-subbuild/lace-populate-prefix/src/lace-populate-stamp/lace-populate-gitinfo.txt"
    "/tmp/sylvan-benchmarks/build-cmake/_deps/lace-subbuild/lace-populate-prefix/src/lace-populate-stamp/lace-populate-gitclone-lastrun.txt"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to copy script-last-run stamp file: '/tmp/sylvan-benchmarks/build-cmake/_deps/lace-subbuild/lace-populate-prefix/src/lace-populate-stamp/lace-populate-gitclone-lastrun.txt'")
endif()

