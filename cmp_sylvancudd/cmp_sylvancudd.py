# from graphviz import Source
# s = Source.from_file("test_interact.gv")
# s.view()

import matplotlib.pyplot as plt
import pyperf
import pylab
import scipy.stats as stats

AIGSYNT_EXEC = "/Users/andrejpistek/Developer/UT/sylvan-benchmarks/cmp_sylvancudd/aigsynt"
MODEL_PATH = '/Users/andrejpistek/Developer/UT/sylvan-benchmarks/safety_synthesis/models/driver/driver_d10y.aag'


def display_histogram_scipy(bench, mean, bins):
    values = bench.get_values()
    values = sorted(values)

    if mean:
        fit = stats.norm.pdf(values, bench.mean(), bench.stdev())
        pylab.plot(values, fit, '-o', label='mean-stdev')
    else:
        fit = stats.norm.pdf(values, bench.mean(), bench.stdev())
        pylab.plot(values, fit, '-o', label='mean-stdev')

    plt.legend(loc='upper right', shadow=True, fontsize='x-large')
    pylab.hist(values, bins=bins)
    pylab.show()


def main():
    runner = pyperf.Runner()
    bench = runner.bench_command('asigsynt_benchmark', [AIGSYNT_EXEC, MODEL_PATH, '-w 4'])
    print(bench.get_values())
    # display_histogram_scipy(bench, None, None)


if __name__ == "__main__":
    main()
