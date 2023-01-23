#!/usr/bin/env Rscript

# one only has to run this once, so that it installs the thing:
#install.packages("remotes")
#remotes::install_github("bramkuijper/simulation.utils")
library("simulation.utils")
parameter_object <- list()

parameter_object$a <- 1.0
parameter_object$b <- 0.01
parameter_object$c <- seq(0,0.2,0.01)
parameter_object$biast <- 0.05
parameter_object$mu_p <- 0.01
parameter_object$mu_t <- 0.01
parameter_object$sdmu_p <- 0.05
parameter_object$sdmu_t <- 0.05
parameter_object$sex_limited_p <- 1.0
parameter_object$sex_limited_t <- 1.0
parameter_object$pref_type <- 0
parameter_object$web <- 1

#purlease continue adding all the parameters, except for the output file
# (this will be done by the function itself)

# please be sensible with the amount of parameters

make.batch.file(parameter_list=parameter_object
                     ,executable_path="./my_simulation.exe")
