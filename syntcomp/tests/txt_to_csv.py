import re
from dataclasses import dataclass
from dataclass_csv import DataclassWriter
from pathlib import Path

results = Path('./results')
results_manu = results / 'manu'
results_auto = results / 'auto'


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


if __name__ == "__main__":
    manu_size_benchmarks = load_size_benchmarks(results_manu)
    write_size_benchmarks_to_csv(manu_size_benchmarks)
    auto_size_benchmarks = load_size_benchmarks(results_auto)
    write_size_benchmarks_to_csv(auto_size_benchmarks)