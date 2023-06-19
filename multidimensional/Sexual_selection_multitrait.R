#!/usr/bin/env Rscript
# plots everything from a single sim 

library("tidyverse")
library("patchwork")
library("ggplot2")

# obtain the final line of the actual simulation data of each file
get_last_line_number <- function(file_name)
{
  # get all the lines
  all_lines <- readLines(con=file_name)
  
  # obtain number of lines
  nlines <- length(all_lines)
  
  # search for last line from the end
  # (coz faster)
  for (line_idx in seq(nlines,1,-1))
  {
    # find the line that starts with a number using grep()
    # if grep returns something, anything, its length > 0
    # hence the return the index of that line and we are done
    if (length(grep(pattern="^\\d",all_lines[[line_idx]]))>0)
    {
      return(line_idx)
    }
  }
  
  # no line means no data hence quit
  stop("no line found")
}


file.name <- commandArgs(trailingOnly=T)[[1]]
# get the last line
last.line <- get_last_line_number(file_name = file.name)

# read stuff into a Hadleyverse tibble
sim_data <- read_delim(file=file.name, 
                       delim=";", 
                       n_max=last.line - 1)

print(head(sim_data))

#Plot mean t
plot_mean_t <- ggplot(data=sim_data
                      ,mapping=aes(x=generation
                                   ,y=mean_t)) +
  geom_line(mapping=aes(x=generation,y=mean_t))

#Plot mean p in a separate graph
plot_mean_p <- ggplot(data=sim_data
                      ,mapping=aes(x=generation
                                   ,y=mean_p)) +
  geom_line(mapping=aes(x=generation,y=mean_p))

#Plot dynamics between p and t
plot_p_t <- ggplot(data=sim_data, aes(x=mean_p, y=mean_t)) + geom_point(size=1) +
  geom_path(mapping=aes(x=mean_p,y=mean_t))

#Plot covarience over time
plot_cov <- ggplot(data=sim_data
                   ,mapping=aes(x=generation
                                ,y=covpt)) +
  geom_line(mapping=aes(x=generation,y=covpt))

plot_mean_t / plot_mean_p / plot_p_t / plot_cov

ggsave(file=paste0("model_graph_Fish_",basename(file.name),".pdf"))

stop()
