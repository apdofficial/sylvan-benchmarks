# A simple BDD based safety game solver for the [Reactive Synthesis Competition](http://www.syntcomp.org/).

This folder contains BDD solver created by [Adam Walker](https://github.com/adamwalker) to whom are all rights are reserved. The solver won the 2014, 2015, 2016 and 2017 sequential realizability tracks.

The solver is written in Haskell and contains Haskell bindings to CUDD and Sylvan bdd packages written also by Adam Walker.

This fodler serves as a benchmark to compare the dynamic variable reordering between Sylvan and CUDD.

### Usage
Install **hyperfine** and execute the following command in the current directory:
```shell
hyperfine -L benchmark models/toy_examples/add10y.aag,models/toy_examples/add12y.aag,models/toy_examples/add14y.aag,models/toy_examples/add16y.aag,models/toy_examples/add18y.aag,models/toy_examples/add20y.aag -L solver run-sylvan,run-cudd --warmup 1 'make {solver} model={benchmark}' --export-json sylvan_cudd_safety.json
```

```shell
hyperfine -L benchmark models/toy_examples/add8y.aag,models/toy_examples/add12y.aag,models/toy_examples/add16y.aag,models/toy_examples/add20y.aag,models/toy_examples/add24y.aag,models/toy_examples/add24y.aag --warmup 1 'make run-sylvan model={benchmark}' --export-json sylvan_cudd_safety.json
```