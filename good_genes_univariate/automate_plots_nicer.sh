#!/usr/bin/env bash 

find . -iname "sim_good_genes_*" -exec ./Sexual_selection_model.R {} \;

rm -rf Rplots.pdf
