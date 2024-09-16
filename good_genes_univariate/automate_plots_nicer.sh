#!/usr/bin/env bash 

# find all the output files that match this grub and then perform an Rscript on them
# please change pattern at will to match what you are having
find . -iname "sim_good_genes_13*" -exec ./Sexual_selection_model.R {} \;
