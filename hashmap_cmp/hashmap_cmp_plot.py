import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns

sns.set_theme(style="darkgrid")

X = "table_usage"
Y = "runtime_ns"
DATA = "./data"
FIGURES = "./figures"

if __name__ == "__main__":
    workers = 5
    ncols = 2
    nrows = int(workers / ncols)

    fig, ax = plt.subplots(ncols=ncols, nrows=nrows, figsize=(10, 10))

    row = 0
    col = 0
    for w in range(1, workers):
        sns.lineplot(x=X,
                     y=Y,
                     data=pd.read_csv(f"{DATA}/w{w}_chaining.csv"),
                     legend='brief',
                     label=f"w={w} Chaining",
                     ax=ax[col][row])
        sns.lineplot(x=X,
                     y=Y,
                     data=pd.read_csv(f"{DATA}/w{w}_probing_limited.csv"),
                     legend='brief',
                     label=f"w={w} Probing limited",
                     ax=ax[col][row])
        sns.lineplot(x=X,
                     y=Y,
                     data=pd.read_csv(f"{DATA}/w{w}_probing_unlimited.csv"),
                     legend='brief',
                     label=f"w={w} Probing unlimited",
                     ax=ax[col][row])
        col += 1
        if col % ncols == 0:
            row += 1
            col = 0

    plt.savefig(f"{FIGURES}/hashmap_cmp.png")
    plt.show()