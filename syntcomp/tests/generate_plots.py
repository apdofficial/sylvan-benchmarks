import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import seaborn as sns
import tikzplotlib
from pathlib import Path

from pandas import DataFrame

sns.set_style("whitegrid")
sns.set_context("notebook", font_scale=1.5, rc={"lines.linewidth": 1.0})

results = Path('./results')
results_manu = results / 'manu'
results_auto = results / 'auto'


if __name__ == "__main__":
    print()