#!/usr/bin/env Rscript

#biasv=c(0.5,0.6,0.7,0.8,0.9,0.99)
#c(seq(0.5,0.9,0.1),0.99)
biasv = c(seq(0.5,0.9,0.1),0.99)
c = 0.2
b = 0.001
init_t = 0
init_p = 3
nrep = 1

maxgen = 10000

pref = seq(0,1,1)
ntrait = 2


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
  for(c_i in c)
  {
    for(b_i in b)
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
        counter <- counter + 1
        file_name_i <- paste0(output_file_prefix,"_",counter)

        echo_str <- paste("echo",counter)

        command_str <- paste(exe,
                        biasv_i,
                        c_i,
                        b_i,
                        init_t_i,
                        init_p_i,
                        format(maxgen,scientific=F),
                        pref_i,
                        ntrait_i,
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

writeLines(text=batch_file_contents)
