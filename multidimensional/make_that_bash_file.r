#!/usr/bin/env Rscript

# one only has to run this once, so that it installs the thing:
#install.packages("remotes")
#remotes::install_github("bramkuijper/simulation.utils")
library("simulation.utils")
parameter_object <- list()

parameter_object$a <- 1    #Choice slope
parameter_object$b <- 0.001 #0.001      #Cost of preference
parameter_object$c <- 0.2 #0.2    #Cost of trait
parameter_object$lambda <- 1    #Keep as 1!!!
parameter_object$biast <- 0.9 #seq(0.5,1,0.02)       #Mutation bias
parameter_object$mu_p <- 0.05
parameter_object$mu_t <- 0.05
parameter_object$sdmu_p <- 0.05
parameter_object$sdmu_t <- 0.05
parameter_object$sex_limited_p <- 1.0
parameter_object$sex_limited_t <- 1.0
parameter_object$pref_type <- seq(0,3,3)
parameter_object$init_t <- 0
parameter_object$init_p <- 3
parameter_object$gam <- 2 #2
parameter_object$thet <- 1 #1
parameter_object$ntrait <- 5

#purlease continue adding all the parameters, except for the output file
# (this will be done by the function itself)

# please be sensible with the amount of parameters
   
make.batch.file(parameter_list=parameter_object
                     ,executable_path="./Weber_multiple.exe",n_replicates=5)
                     
                     
#seq(0,0.5,0.1)
