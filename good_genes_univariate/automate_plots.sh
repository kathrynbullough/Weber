#!/usr/bin/env bash

<<<<<<< HEAD
find . -iname "sim_good_genes_12*" -print0 | xargs -0 -P10 -I% ./plot_output.r %
=======
find . -iname "sim_good_genes_*" -print0 | xargs -0 -P10 -I% ./plot_output.r %
>>>>>>> 1919f84dedc9886ed744eb60165a8f2be5d4ec77

rm -rf Rplots.pdf
