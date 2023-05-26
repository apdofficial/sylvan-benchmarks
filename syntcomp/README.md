# A simple BDD based safety game solver for the [Reactive Synthesis Competition](http://www.syntcomp.org/).

This folder contains BDD solver created by [Adam Walker](https://github.com/adamwalker) to whom are all rights are reserved. The soler won the 2014, 2015, 2016 and 2017 sequential realizability tracks.

The solver is written in Haskell and contains Haskell bindings to CUDD and Sylvan bdd packages written also by Adam Walker.

This fodler serves as a benchamrk to compare the dynamic variable reordering between Sylvan and CUDD.

### Usage
Install **hyperfine** and execute the following command in the current directory:
```shell
hyperfine -L benchmark moving_obstacle_8x8_1glitches,add14y -L solver sylvan,cudd --warmup 1 'make {solver} model={benchmark}.aag'
```
