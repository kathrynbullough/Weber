#!/usr/bin/env Rscript

library("devtools")
devtools::install_github("bramkuijper/simulation.utils")
library("simulation.utils")
library("readr")

all.bloody.data <- summarize.sims(simulations_path="."
        ,simulation_file_pattern="sim_good_genes_12\\d"
        ,parameter_start_pattern="^seed"
        ,data_end_pattern="^$"
        )

# some shizzle like this to get the file on disk
write_delim(file="gg_uni.csv", x=all.bloody.data)
#test
