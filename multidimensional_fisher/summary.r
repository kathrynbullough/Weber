#!/usr/bin/env Rscript

library("devtools")
devtools::install_github("bramkuijper/simulation.utils")

library("simulation.utils")
library("readr")

all.bloody.data <- summarize.sims(simulations_path="."
        ,simulation_file_pattern="output_\\d"
        ,parameter_start_pattern="^type"
        ,data_end_pattern="^$"
        )

# some shizzle like this to get the file on disk
write_delim(file="fish_gamthetry2.csv", x=all.bloody.data)
