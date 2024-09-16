#!/usr/bin/env Rscript

library("simulation.utils")
library("readr")

all.bloody.data <- summarize.sims(simulations_path="."
        ,simulation_file_pattern="sim_good_genes_13092024_12\\d"
        ,parameter_start_pattern="^seed"
        ,data_end_pattern="^$"
        )

# some shizzle like this to get the file on disk
write_delim(file="oe_web_bias_n2.csv", x=all.bloody.data)
#test
