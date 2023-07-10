#!/usr/bin/env Rscript

# one only has to run this once, so that it installs the thing:
#install.packages("remotes")
#remotes::install_github("bramkuijper/simulation.utils")
library("simulation.utils")
parameter_object <- list()

parameter_object$a <- 1    #Choice slope
parameter_object$b <- 0.001      #Cost of preference
parameter_object$c <- 0.5      #Cost of trait
parameter_object$biast <- 0.5  #Mutation bias
parameter_object$mu_p <- 0.0
parameter_object$mu_t <- 0.0
parameter_object$sdmu_p <- 0.0
parameter_object$sdmu_t <- 0.0
parameter_object$sex_limited_p <- 1.0
parameter_object$sex_limited_t <- 1.0
parameter_object$pref_type <- 0
parameter_object$web <- 0
parameter_object$init_t <- 0
parameter_object$init_p <- 10

#purlease continue adding all the parameters, except for the output file
# (this will be done by the function itself)

# please be sensible with the amount of parameters

make.batch.file(parameter_list=parameter_object
                     ,executable_path="./Weber.exe",n_replicates=1)
                     
                     
#seq(0,0.5,0.1)
