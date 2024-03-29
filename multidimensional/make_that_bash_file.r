#!/usr/bin/env Rscript

# one only has to run this once, so that it installs the thing:
#install.packages("remotes")
#remotes::install_github("bramkuijper/simulation.utils")
library("simulation.utils")
parameter_object <- list()

parameter_object$a <- 1    #Choice slope
parameter_object$b <- 0.001 #seq(0.001,0.025,0.006)      #Cost of preference
parameter_object$c1 <- 0.2 #seq(0.01,0.99,0.245)     #Cost of trait
parameter_object$c2 <- 0.2 #seq(0.01,0.99,0.245)
parameter_object$lambda <- 1    #Keep as 1!!!
parameter_object$biast1 <- 0.9 #seq(0.5,1,0.02)       #Mutation bias
parameter_object$biast2 <- 0.9 #seq(0.5,1,0.02)
parameter_object$mu_p1 <- 0.05
parameter_object$mu_p2 <- 0.05
parameter_object$mu_t1 <- 0.05
parameter_object$mu_t2 <- 0.05
parameter_object$sdmu_p1 <- 0.05
parameter_object$sdmu_p2 <- 0.05
parameter_object$sdmu_t1 <- 0.05
parameter_object$sdmu_t2 <- 0.05
parameter_object$sex_limited_p <- 1.0
parameter_object$sex_limited_t <- 1.0
parameter_object$pref_type <- 0
parameter_object$init_t1 <- 0
parameter_object$init_t2 <- 0
parameter_object$init_p1 <- 3
parameter_object$init_p2 <- 5
parameter_object$gam <- 2 #does 1 look better?
parameter_object$thet <- 1

#purlease continue adding all the parameters, except for the output file
# (this will be done by the function itself)

# please be sensible with the amount of parameters
   
make.batch.file(parameter_list=parameter_object
                     ,executable_path="./Weber_multiple.exe",n_replicates=10)
                     
                     
#seq(0,0.5,0.1)
