#!/usr/bin/env bash

find . -iname "sim_good_genes_16*" -print0 | xargs -0 -P10 -I% ./plot_output.r %

rm -rf Rplots.pdf
