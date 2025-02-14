#!/usr/bin/env bash 

find . -iname "sim_good_genes_06*" -exec ./Sexual_selection_model.R {} \;

rm -rf Rplots.pdf
