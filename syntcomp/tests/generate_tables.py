import pandas as pd
from pathlib import Path
from pandas import DataFrame

results = Path('./results')
results_manu = results / 'manu'
results_auto = results / 'auto'


def filter_size_benchmark(path: Path) -> bool:
    is_cudd = path.name.startswith("cudd")
    is_sylvan = path.name.startswith("sylvan")
    return is_cudd or is_sylvan


def load_size_benchmarks(path: Path) -> DataFrame:
    size_results = filter(filter_size_benchmark, list(path.glob('*.csv')))
    frames: [DataFrame] = []
    for path in size_results:
        frame = pd.read_csv(path)
        frame["benchmark"] = path.name.split(".")[0]
        frames.append(frame)
    return pd.concat(frames)


def write_dataframe_to_latex_table(df: DataFrame, file: Path):
    with open(file, "w") as tf:
        tf.write(df.to_latex())


if __name__ == "__main__":
    manu_size_benchmarks = load_size_benchmarks(results_manu)
    print(manu_size_benchmarks.head)
    write_dataframe_to_latex_table(manu_size_benchmarks, results / "manu_size_benchmarks.tex")

    auto_size_benchmarks = load_size_benchmarks(results_auto)
    print(auto_size_benchmarks.head)
    write_dataframe_to_latex_table(auto_size_benchmarks, results / "auto_size_benchmarks.tex")