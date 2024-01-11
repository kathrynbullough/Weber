#!/usr/bin/env bash 

# find all the output files that match this grub and then perform an Rscript on them
# please change pattern at will to match what you are having
find . -iname "outputWEB_*" -exec ./Sexual_selection_model.R {} \;
