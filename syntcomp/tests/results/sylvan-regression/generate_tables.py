from dataclasses import dataclass
import re
import pandas as pd
from pathlib import Path
from pandas import DataFrame
from dataclass_csv import DataclassWriter

cwd = Path("./")
results_quality = cwd / "quality"
results_runtime = cwd / "runtime"


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


def load_quality_df(path: Path) -> DataFrame:
    quality_results = list(path.glob('*.csv'))
    frames: [DataFrame] = []
    for path in quality_results:
        frame = pd.read_csv(path)
        nt, mg, mv, ms, rt, model = path.name.split("-")
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


def load_runtime_df(path: Path) -> DataFrame:
    size_results = filter(filter_runtime_benchmark, list(path.glob('*.csv')))
    frames: [DataFrame] = []
    for path in size_results:
        frame = pd.read_csv(path)
        frames.append(frame)
    return pd.concat(frames)


def generate_quality_table():
    # convert the txt files to csv files for the quality benchmarks
    quality_benchmarks = load_quality_benchmarks(results_quality)
    write_quality_benchmarks_to_csv(quality_benchmarks)

    quality_benchmarks = load_quality_df(results_quality)
    quality_benchmarks.to_csv(cwd / "quality_benchmarks.csv", encoding='utf-8', index=False)


def generate_runtime_table():
    runtime_benchmarks = load_runtime_df(results_runtime)
    # runtime_benchmarks = runtime_benchmarks.drop("command", axis=1)
    runtime_benchmarks.columns = [col.replace('parameter_', '') for col in runtime_benchmarks.columns]
    runtime_benchmarks["model"] = runtime_benchmarks.apply(lambda row: row["model"].split("/")[-1], axis=1)
    runtime_benchmarks.to_csv(cwd / "runtime_benchmarks.csv", encoding='utf-8', index=False)


if __name__ == "__main__":
    generate_quality_table()
    generate_runtime_table()
