#!/usr/bin/env Rscript

library("simulation.utils")

all.bloody.data <- summarize.sims(simulations_path="."
        ,simulation_file_pattern="output_fisher\\d.csv"
        ,parameter_start_pattern="^type"
        ,data_end_pattern="^$"
        )

# some shizzle like this to get the file on disk
write_delim(file="output.csv", x=all.bloody.data)
