from dataclasses import dataclass
import re
import pandas as pd
from pathlib import Path
from pandas import DataFrame
from dataclass_csv import DataclassWriter

cwd = Path("/Users/andrejpistek/Developer/MSc/sylvan-benchmarks/syntcomp/tests/results")
results_manu = cwd / "manu"
results_auto = cwd / "auto"


@dataclass
class Reordering:
    from_size: int
    to_size: int
    time: float


@dataclass
class Benchmark:
    path: Path
    reorderings: [Reordering]


def txt_to_benchmark(path: Path) -> Benchmark:
    with open(path, 'r') as f:
        lines = f.readlines()
        lines = [line.strip() for line in lines if line.startswith("BDD")]
        benchmark = Benchmark(path, [])
        for line in lines:
            line = re.findall(r'\b\d+\b', line)
            if len(line) == 4:
                benchmark.reorderings.append(Reordering(int(line[0]), int(line[1]), float(line[2] + "." + line[3])))
            else:
                benchmark.reorderings.append(Reordering(int(line[0]), int(line[1]), int(line[2])))
        return benchmark


def load_size_benchmarks(path: Path) -> [Benchmark]:
    size_results = list(path.glob('*.txt'))
    benchmarks: [Benchmark] = []
    for path in size_results:
        benchmarks.append(txt_to_benchmark(path))
    return benchmarks


def write_size_benchmarks_to_csv(benchmarks: [Benchmark]):
    for benchmark in benchmarks:
        with open(benchmark.path.with_suffix('.csv'), 'w') as f:
            w = DataclassWriter(f, benchmark.reorderings, Reordering)
            w.write()


def filter_quality_benchmark(path: Path) -> bool:
    is_cudd = path.name.startswith("cudd")
    is_sylvan = path.name.startswith("sylvan")
    return is_cudd or is_sylvan


def load_quality_benchmarks(path: Path) -> DataFrame:
    size_results = filter(filter_quality_benchmark, list(path.glob('*.csv')))
    frames: [DataFrame] = []
    for path in size_results:
        frame = pd.read_csv(path)
        frame["benchmark"] = path.name.split(".")[0]
        frames.append(frame)
    return pd.concat(frames)


def filter_runtime_benchmark(path: Path) -> bool:
    is_cudd = path.name.startswith("cudd")
    is_sylvan = path.name.startswith("sylvan")
    return not is_cudd and not is_sylvan


def load_runtime_benchmarks(path: Path) -> DataFrame:
    size_results = filter(filter_runtime_benchmark, list(path.glob('*.csv')))
    frames: [DataFrame] = []
    for path in size_results:
        frame = pd.read_csv(path)
        frames.append(frame)
    return pd.concat(frames)


if __name__ == "__main__":
    # convert the txt files to csv files for the quality benchmarks
    manu_size_benchmarks = load_size_benchmarks(results_manu)
    write_size_benchmarks_to_csv(manu_size_benchmarks)
    auto_size_benchmarks = load_size_benchmarks(results_auto)
    write_size_benchmarks_to_csv(auto_size_benchmarks)

    manu_quality_benchmarks = load_quality_benchmarks(results_manu)
    print(manu_quality_benchmarks.head())
    manu_quality_benchmarks.to_csv(cwd / "manu_quality_benchmarks.csv", encoding='utf-8', index=False)

    auto_quality_benchmarks = load_quality_benchmarks(results_auto)
    print(auto_quality_benchmarks.head())
    auto_quality_benchmarks.to_csv(cwd /"auto_quality_benchmarks.csv", encoding='utf-8', index=False)

    manu_runtime_benchmarks = load_runtime_benchmarks(results_manu)
    print(manu_runtime_benchmarks.head())
    manu_runtime_benchmarks.to_csv(cwd /"manu_runtime_benchmarks.csv", encoding='utf-8', index=False)

    auto_runtime_benchmarks = load_runtime_benchmarks(results_auto)
    print(auto_runtime_benchmarks.head())
    auto_runtime_benchmarks.to_csv(cwd /"auto_runtime_benchmarks.csv", encoding='utf-8', index=False)