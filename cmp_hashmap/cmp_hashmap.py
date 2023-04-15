import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import seaborn as sns

sns.set_style("whitegrid")
sns.set_context("notebook", font_scale=1.5, rc={"lines.linewidth": 1.0})

X = "table_usage"
Y = "runtime_ms"
DATA = "../build/hashmap_cmp"
FIGURES = "./figures"


def linspace(start, stop, step=1.):
    """
    Like np.linspace but uses step instead of num
    This is inclusive to stop, so if start=1, stop=3, step=0.5
    Output is: array([1., 1.5, 2., 2.5, 3.])
    """
    return np.linspace(start, stop, int((stop - start) / step + 1))


if __name__ == "__main__":
    nworkers = 4
    ncols = 2
    nrows = int(nworkers / ncols)

    fig, ax = plt.subplots(ncols=ncols, nrows=nrows, figsize=(15, 15))

    row = 0
    col = 0

    for w in range(1, nworkers + 1):
        try:
            gfg = sns.lineplot(x=X,
                               y=Y,
                               data=pd.read_csv(f"{DATA}/w{w}_chaining.csv"),
                               legend='brief',
                               label=f"w={w} Chaining",
                               ax=ax[row][col])
        except FileNotFoundError:
            pass

        try:
            gfg = sns.lineplot(x=X,
                               y=Y,
                               data=pd.read_csv(f"{DATA}/w{w}_probing_limited.csv"),
                               legend='brief',
                               label=f"w={w} Probing limited",
                               ax=ax[row][col])
        except FileNotFoundError:
            pass

        try:
            gfg = sns.lineplot(x=X,
                               y=Y,
                               data=pd.read_csv(f"{DATA}/w{w}_probing_unlimited.csv"),
                               legend='brief',
                               label=f"w={w} Probing unlimited",
                               ax=ax[row][col])
        except FileNotFoundError:
            pass

        gfg.set_yticks(linspace(start=0, stop=110, step=15))
        gfg.set_xticks(linspace(start=0, stop=100, step=10))
        gfg.legend(loc='upper left')
        gfg.set(ylabel="Runtime (ms)", xlabel="Used table (%)")

        col += 1
        if col % ncols == 0:
            row += 1
            col = 0

    plt.savefig(f"{FIGURES}/hashmap_cmp.svg")