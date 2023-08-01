suppressPackageStartupMessages(library('tidyverse'))
suppressPackageStartupMessages(library('ggplot2'))
suppressPackageStartupMessages(library('tikzDevice'))
suppressPackageStartupMessages(library('xtable'))
suppressPackageStartupMessages(library('lemon'))
suppressPackageStartupMessages(library('knitr'))
suppressPackageStartupMessages(library('scales'))

# Read input data
# For timeouts, "States" field is set to -1
manu_quality_benchmarks <- read_delim('results/manu_quality_benchmarks.csv')

# filter out the data by group of two solvers compared with each other in scatter plot

ggplot(manu_quality_benchmarks, aes(x=to_size, y=solver)) + geom_point()