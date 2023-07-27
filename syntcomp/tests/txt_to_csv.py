import re
from dataclasses import dataclass
from dataclass_csv import DataclassWriter
from pathlib import Path

results = Path('./results')
results_manu = results / 'manu'
results_auto = results / 'auto'


@dataclass
class Size:
    from_size: int
    to_size: int
    time: float


@dataclass
class SizeBenchmark:
    path: Path
    results: [Size]


def txt_size_benchmark(path: Path) -> SizeBenchmark:
    with open(path, 'r') as f:
        lines = f.readlines()
        lines = [line.strip() for line in lines if line.startswith("BDD")]
        benchmark = SizeBenchmark(path, [])
        for line in lines:
            line = re.findall(r'\b\d+\b', line)
            if len(line) == 4:
                benchmark.results.append(Size(int(line[0]), int(line[1]), float(line[2] + "." + line[3])))
            else:
                benchmark.results.append(Size(int(line[0]), int(line[1]), int(line[2])))
        return benchmark


def extract_size_benchmarks(path: Path) -> [SizeBenchmark]:
    size_results = list(path.glob('*.txt'))
    benchmarks: [SizeBenchmark] = []
    for path in size_results:
        benchmarks.append(txt_size_benchmark(path))
    return benchmarks


def write_benchmarks_to_csv(benchmarks: [SizeBenchmark]):
    for benchmark in benchmarks:
        with open(benchmark.path.with_suffix('.csv'), 'w') as f:
            w = DataclassWriter(f, benchmark.results, Size)
            w.write()


if __name__ == "__main__":
    manu_size_benchmarks = extract_size_benchmarks(results_manu)
    write_benchmarks_to_csv(manu_size_benchmarks)
    auto_size_benchmarks = extract_size_benchmarks(results_auto)
    write_benchmarks_to_csv(auto_size_benchmarks)