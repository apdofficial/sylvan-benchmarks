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
    size_results = list(path.glob('*.txt'))
    benchmarks: [Benchmark] = []
    for path in size_results:
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
        if len(path.name.split("-")) == 8:
            solver, workers, nt, mg, mv, ms, rt, model = path.name.split("-")
            frame["solver"] = solver
            frame["heuristic"] = "sift"
            frame["workers"] = workers
            frame["nodes_threshold"] = nt
            frame["max_growth"] = mg
            frame["max_var"] = mv
            frame["max_swap"] = ms
            frame["reordering_trigger"] = rt
            frame["model"] = model.split(".")[0]
            frames.append(frame)
        elif len(path.name.split("-")) == 4:
            solver, rt, h, model = path.name.split("-")
            frame["solver"] = solver
            frame["heuristic"] = h
            frame["workers"] = "NaN"
            frame["nodes_threshold"] = "NaN"
            frame["max_growth"] = "NaN"
            frame["max_var"] = "NaN"
            frame["max_swap"] = "NaN"
            frame["reordering_trigger"] = rt
            frame["model"] = model.split(".")[0]
            frames.append(frame)
    return pd.concat(frames)


def load_runtime_df(path: Path) -> DataFrame:
    size_results = list(path.glob('*.csv'))
    frames: [DataFrame] = []
    for path in size_results:
        frame = pd.read_csv(path)
        frames.append(frame)
    return pd.concat(frames)


def generate_quality_table():
    # convert the txt files to csv files for the quality benchmarks
    quality = load_quality_benchmarks(results_quality)
    write_quality_benchmarks_to_csv(quality)

    quality = load_quality_df(results_quality)
    quality.to_csv(cwd / "sylvan_cudd_quality.csv", encoding='utf-8', index=False)


def derive_heuristic(row):
    if str(row["solver"]).__contains__("sift"):
        return "sift"
    elif str(row["solver"]).__contains__("group"):
        return "group"
    elif str(row["solver"]).__contains__("sylvan"):
        return "sift"
    else:
        return row["heuristic"]


def generate_runtime_table():
    runtime = load_runtime_df(results_runtime)
    # runtime = runtime.drop("command", axis=1)
    runtime.columns = [col.replace('parameter_', '') for col in runtime.columns]
    runtime["model"] = runtime.apply(lambda row: row["model"].split("/")[-1], axis=1)
    runtime["heuristic"] = runtime.apply(derive_heuristic, axis=1)
    runtime["solver"] = runtime.apply(lambda row: row["solver"].replace("-solver", ""), axis=1)
    runtime.to_csv(cwd / "sylvan_cudd_runtime.csv", encoding='utf-8', index=False)


if __name__ == "__main__":
    generate_quality_table()
    generate_runtime_table()
