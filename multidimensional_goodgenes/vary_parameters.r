#!/usr/bin/env Rscript

biasv = c(seq(0.5,0.9,0.1),0.99)
#c(seq(0.5,0.9,0.1),0.99)
#biasv = 0.99
#c = c(0.01,0.255,0.5,0.745,0.99)
#b = c(0.01,0.07,0.013,0.019,0.025)
a = 1.0
b = c(0.0025)  #0.0025
c = 0.5  #0.2  #0.5

gam = 2.0 #c(0.1,1.0,2.0,3.0) 
thet = 1.0 #c(0.1,0.2,0.5,0.8)  #c(0.59,0.61)

weber_k <- c(0)

init_t = 0
init_p = 1
nrep = 20

maxgen = 50000

pref = c(1)
ntrait = 1


# generate a date_time stamp as a character
date_str <- format(Sys.time(), "%d%m%Y_%H%M%S")

# generate names for the output file based on date and time 
# to ensure some sort of uniqueness, to avoiding that output
# files from different runs overwrite eachother
output_file_prefix = paste0("sim_good_genes_",date_str)

counter <- 0

exe = "./good_genes.exe"

batch_file_contents <- ""

for (rep_i in 1:nrep)
{
    for (a_i in a)
    {
      for(c_i in c)
      {
            for(b_i in b)
            {
		for(gam_i in gam)
		{
			for(thet_i in thet)
			{
        		    for(init_t_i in init_t)
           		    {
           			 for(init_p_i in init_p)
           			 {
             				 for(pref_i in pref)
             				 {
               					 for(ntrait_i in ntrait)
               					 {
                 					 for (biasv_i in biasv)
                 					 {
                                         for (weber_k_i in weber_k)
                                         {
                                            counter <- counter + 1
                                            file_name_i <- paste0(output_file_prefix,"_",counter)

                                            echo_str <- paste("echo",counter)

                                            command_str <- paste(exe,
                                                            biasv_i,
                                                            a_i,
                                                            b_i,
                                                            c_i,
                                                            gam_i,
                                                            thet_i,
                                                            init_t_i,
                                                            init_p_i,
                                                            format(maxgen,scientific=F),
                                                            pref_i,
                                                            ntrait_i,
                                                            weber_k_i,
                                                            file_name_i)

                                            # append to batch file contents
                                            batch_file_contents <- paste0(batch_file_contents
                                                    ,"\n"
                                                    ,echo_str
                                                    ,"\n"
                                                    ,command_str)
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        } # end b
      } # end c
    } # end a 
} # end for loop

writeLines(text=batch_file_contents)
