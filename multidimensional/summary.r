#!/usr/bin/env Rscript

library("simulation.utils")
library("readr")

all.bloody.data <- summarize.sims(simulations_path="."
        ,simulation_file_pattern="output_\\d"
        ,parameter_start_pattern="^type"
        ,data_end_pattern="^$"
        )

# some shizzle like this to get the file on disk
write_delim(file="thetbc_graph.csv", x=all.bloody.data)
