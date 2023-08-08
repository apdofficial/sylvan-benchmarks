from dataclasses import dataclass
import re
import pandas as pd
from pathlib import Path
from pandas import DataFrame
from dataclass_csv import DataclassWriter

results = Path("./slurm/results")
results_quality = results / "data" / "sylvan-regression" / "quality"
results_runtime = results / "data" / "sylvan-regression" / "runtime"


# results = Path("./results")
# results_quality = results / "sylvan-regression" / "quality"
# results_runtime = results / "sylvan-regression" / "runtime"


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
            elif len(line) == 3:
                benchmark.reorderings.append(Reordering(int(line[0]), int(line[1]), int(line[2])))
        return benchmark


def load_quality_benchmarks(path: Path) -> [Benchmark]:
    quality_results = list(path.glob('*.txt'))
    benchmarks: [Benchmark] = []
    for path in quality_results:
        benchmarks.append(txt_to_benchmark(path))
    return benchmarks


def write_quality_benchmarks_to_csv(benchmarks: [Benchmark]):
    for benchmark in benchmarks:
        with open(benchmark.path.with_suffix('.csv'), 'w') as f:
            w = DataclassWriter(f, benchmark.reorderings, Reordering)
            w.write()


def filter_quality_benchmark(path: Path) -> bool:
    is_cudd = path.name.startswith("cudd")
    is_sylvan = path.name.startswith("sylvan")
    return is_cudd or is_sylvan


def load_quality_df(path: Path, type: str) -> DataFrame:
    quality_results = list(path.glob(f"*{type}-*.csv"))
    frames: [DataFrame] = []
    for path in quality_results:
        frame = pd.read_csv(path)
        n, nt, mg, mv, ms, rt, model, i = path.name.split("-")
        frame["workers"] = n
        frame["nodes_threshold"] = nt
        frame["max_growth"] = mg
        frame["max_var"] = mv
        frame["max_swap"] = ms
        frame["reordering_trigger"] = rt
        frame["model"] = model.split(".")[0]
        frames.append(frame)
    return pd.concat(frames)


def filter_runtime_benchmark(path: Path) -> bool:
    is_cudd = path.name.startswith("cudd")
    is_sylvan = path.name.startswith("sylvan")
    return not is_cudd and not is_sylvan


def load_runtime_df(path: Path, type: str) -> DataFrame:
    size_results = filter(filter_runtime_benchmark, list(path.glob(f"*{type}-*.csv")))
    frames: [DataFrame] = []
    for path in size_results:
        frame = pd.read_csv(path)
        frames.append(frame)
    return pd.concat(frames)


if __name__ == "__main__":
    # convert the txt files to csv files for the quality benchmarks
    quality = load_quality_benchmarks(results_quality)
    write_quality_benchmarks_to_csv(quality)

    quality = load_quality_df(results_quality, "max_growth")
    quality.to_csv(results / "sylvan_regression_max_growth.csv", encoding='utf-8', index=False)

    quality = load_quality_df(results_quality, "nodes_threshold")
    quality.to_csv(results / "sylvan_regression_nodes_threshold.csv", encoding='utf-8', index=False)

    quality = load_quality_df(results_quality, "workers")
    quality.to_csv(results / "sylvan_regression_workers.csv", encoding='utf-8', index=False)
