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

#  a       b     c     biast       mu_p    mu_t    sdmu_p    sdmu_t   sex-limited p       sex-limited t
# 1.0     0.5    0.5     0.5        0.05    0.05    0.4        0.4        0                 0             

fisher1 <- read.delim("fisher1.csv", sep=";", header=T)
fisher2 <- read.delim("fisher2.csv", sep=";", header=T)
fisher3 <- read.delim("fisher3.csv", sep=";", header=T)
fisher4 <- read.delim("fisher4.csv", sep=";", header=T)
fisher5 <- read.delim("fisher5.csv", sep=";", header=T)

plot(fisher1$generation,fisher1$mean_p,type="l",pch=18)
plot(fisher2$generation,fisher2$mean_p,type="l",pch=18)
plot(fisher3$generation,fisher3$mean_p,type="l",pch=18)
plot(fisher4$generation,fisher4$mean_p,type="l",pch=18)
plot(fisher5$generation,fisher5$mean_p,type="l",pch=18)

web1 <- read.delim("web1.csv", sep=";", header=T)
web2 <- read.delim("web2.csv", sep=";", header=T)
web3 <- read.delim("web3.csv", sep=";", header=T)
web4 <- read.delim("web4.csv", sep=";", header=T)
web5 <- read.delim("web5.csv", sep=";", header=T)

plot(web1$generation,web1$mean_p,type="l",pch=18)
plot(web2$generation,web2$mean_p,type="l",pch=18)
plot(web3$generation,web3$mean_p,type="l",pch=18)
plot(web4$generation,web4$mean_p,type="l",pch=18)
plot(web5$generation,web5$mean_p,type="l",pch=18)

#     a       b     c     biast       mu_p    mu_t    sdmu_p    sdmu_t   sex-limited p       sex-limited t
#   1.0     0.25    0.25     0.3        0.05    0.05    0.4        0.4        0.2                 0.2       

fisher21 <- read.delim("fisher21.csv", sep=";", header=T)
fisher22 <- read.delim("fisher22.csv", sep=";", header=T)
fisher23 <- read.delim("fisher23.csv", sep=";", header=T)
fisher24 <- read.delim("fisher24.csv", sep=";", header=T)
fisher25 <- read.delim("fisher25.csv", sep=";", header=T)

plot(fisher21$generation,fisher21$mean_p,type="l",pch=18)
plot(fisher22$generation,fisher22$mean_p,type="l",pch=18)
plot(fisher23$generation,fisher23$mean_p,type="l",pch=18)
plot(fisher24$generation,fisher24$mean_p,type="l",pch=18)
plot(fisher25$generation,fisher25$mean_p,type="l",pch=18)

web21 <- read.delim("web21.csv", sep=";", header=T)
web22 <- read.delim("web22.csv", sep=";", header=T)
web23 <- read.delim("web23.csv", sep=";", header=T)
web24 <- read.delim("web24.csv", sep=";", header=T)
web25 <- read.delim("web25.csv", sep=";", header=T)

plot(web21$generation,web21$mean_t,type="l",pch=18)
plot(web22$generation,web22$mean_t,type="l",pch=18)
plot(web23$generation,web23$mean_t,type="l",pch=18)
plot(web24$generation,web24$mean_t,type="l",pch=18)
plot(web25$generation,web25$mean_t,type="l",pch=18)

#   a       b     c     biast       mu_p    mu_t    sdmu_p    sdmu_t   sex-limited p       sex-limited t
#  0.5     0.25    0.25     0.3        0.05    0.05    0.4        0.4        0.2                 0.2      

fisher31 <- read.delim("fisher31.csv", sep=";", header=T)
fisher32 <- read.delim("fisher32.csv", sep=";", header=T)
fisher33 <- read.delim("fisher33.csv", sep=";", header=T)
fisher34 <- read.delim("fisher34.csv", sep=";", header=T)
fisher35 <- read.delim("fisher35.csv", sep=";", header=T)

plot(fisher31$generation,fisher31$mean_p,type="l",pch=18)
plot(fisher32$generation,fisher32$mean_p,type="l",pch=18)
plot(fisher33$generation,fisher33$mean_p,type="l",pch=18)
plot(fisher34$generation,fisher34$mean_p,type="l",pch=18)
plot(fisher35$generation,fisher35$mean_p,type="l",pch=18)

web31 <- read.delim("web31.csv", sep=";", header=T)
web32 <- read.delim("web32.csv", sep=";", header=T)
web33 <- read.delim("web33.csv", sep=";", header=T)
web34 <- read.delim("web34.csv", sep=";", header=T)
web35 <- read.delim("web35.csv", sep=";", header=T)

plot(web31$generation,web31$mean_p,type="l",pch=18)
plot(web32$generation,web32$mean_p,type="l",pch=18)
plot(web33$generation,web33$mean_p,type="l",pch=18)
plot(web34$generation,web34$mean_p,type="l",pch=18)
plot(web35$generation,web35$mean_p,type="l",pch=18)

par(mfrow=c(5,1))
par(mfrow=c(1,1))

#Female preference cost 0
#Biased mutations 0
#a at 1 or 2

#   a       b                  c          biast             mu_p    mu_t    sdmu_p    sdmu_t   sex-limited p       sex-limited t
#  1 or 2   0 or very low    very low     0 up to 0.5        0.05    0.05    0.4        0.4        1                 1  


sims_output <- read.delim("sims_output.csv", sep=" ", header=T)
plot(sims_output$file,sims_output$mean_p)

sims_output_web <- read.delim("sims_output_web.csv", sep=" ", header=T)
plot(sims_output_web$file,sims_output_web$mean_p)

#   a       b                  c          biast             mu_p    mu_t    sdmu_p    sdmu_t   sex-limited p       sex-limited t
#  1         0               0            0.3                 0.05    0.05    0.4        0.4        1                 1  

sims_output_fish <- read.delim("sims_output_fish.csv", sep=" ", header=T)
plot(sims_output_fish$file,sims_output_fish$mean_p)

sims_output_web2 <- read.delim("sims_output_web2.csv", sep=" ", header=T)
plot(sims_output_web2$file,sims_output_web2$mean_t)


#   a       b                  c          biast             mu_p    mu_t    sdmu_p    sdmu_t   sex-limited p       sex-limited t
#  1         0.01          varies            0.01             0.05    0.05    0.4        0.4        1                 1  

var_c<-read.delim("sims_output_c2.csv", sep=" ", header=T)

plot_c <- ggplot(var_c) + 
          geom_point(aes(x=c.,y=mean_p,colour="p")) + 
          geom_point(aes(x=c.,y=mean_t,colour="t"))
plot_c

var_c2<-read.delim("sims_output_c3.csv", sep=" ", header=T)

plot_c2 <- ggplot(var_c2) + 
  geom_point(aes(x=c.,y=mean_p,colour="p")) + 
  geom_point(aes(x=c.,y=mean_t,colour="t"))
plot_c2

var_c3<-read.delim("sims_output_c4.csv", sep=" ", header=T)

plot_c3 <- ggplot(var_c3) + 
  geom_point(aes(x=c.,y=mean_p,colour="p")) + 
  geom_point(aes(x=c.,y=mean_t,colour="t"))
plot_c3

var_ctest<-read.delim("c_test.csv", sep=" ", header=T)

sum <- summarySE(var_ctest, measurevar="t", groupvars=c("supp","dose"))
plot_ctest <- ggplot(var_ctest) + 
  geom_point(aes(x=c.,y=mean_p,colour="p")) + 
  geom_point(aes(x=c.,y=mean_t,colour="t")) +
  geom_errorbar(width = 0.2)
plot_ctest

#   a       b                  c          biast             mu_p    mu_t    sdmu_p    sdmu_t   sex-limited p       sex-limited t
#  1         0.001          varies            0.01             0.05    0.05    0.4        0.4        1                 1  

var_c4<-read.delim("sims_output_c5.csv", sep=" ", header=T)

plot_c4 <- ggplot(var_c4) + 
  geom_point(aes(x=c.,y=mean_p,colour="p")) + 
  geom_point(aes(x=c.,y=mean_t,colour="t"))
plot_c3

#Five replicates all in one
var_cbulk<-read.delim("sims_output_cbulk.csv", sep=" ", header=T)

plot_cbulk <- ggplot(var_cbulk) + 
  geom_point(aes(x=c.,y=mean_p,colour="p")) + 
  geom_point(aes(x=c.,y=mean_t,colour="t"))
plot_cbulk

#Start with initial t value of 5
var_ct<-read.delim("sims_output_tt.csv", sep=" ", header=T)

plot_ct <- ggplot(var_ct) + 
  geom_point(aes(x=c.,y=mean_p,colour="p")) + 
  geom_point(aes(x=c.,y=mean_t,colour="t"))
plot_ct

#Start with initial t value of 8
var_ct2<-read.delim("sims_output_tt2.csv", sep=" ", header=T)

plot_ct2 <- ggplot(var_ct2) + 
  geom_point(aes(x=c.,y=mean_p,colour="p")) + 
  geom_point(aes(x=c.,y=mean_t,colour="t"))
plot_ct2

#   a       b                  c          biast             mu_p    mu_t    sdmu_p    sdmu_t   sex-limited p       sex-limited t
#  1         0.001          varies            0.01             0.05    0.05    0.1        0.1        1                 1  

#Start with initial t value of 8
var_ct3<-read.delim("sims_output_t8.csv", sep=" ", header=T)

plot_ct3 <- ggplot(var_ct3) + 
  geom_point(aes(x=c.,y=mean_p,colour="p")) + 
  geom_point(aes(x=c.,y=mean_t,colour="t"))
plot_ct3

testtt<-read.delim("testtt.csv", sep=" ", header=T)

plot_testtt <- ggplot(testtt) + 
  geom_point(aes(x=c.,y=mean_p,colour="p")) + 
  geom_point(aes(x=c.,y=mean_t,colour="t"))
plot_testtt

#   a       b                  c          biast             mu_p    mu_t    sdmu_p    sdmu_t   sex-limited p       sex-limited t
#  1         0.01             0.01        varies             0.05    0.05    0.4        0.4        1                 1  

var_bias<-read.delim("sims_output_bias.csv", sep=" ", header=T)

plot_bias <- ggplot(var_bias) + 
  geom_point(aes(x=biast.,y=mean_t,colour="t")) + 
  geom_point(aes(x=biast.,y=mean_p,colour="p"))
plot_bias

#   a       b                  c          biast             mu_p    mu_t    sdmu_p    sdmu_t   sex-limited p       sex-limited t
#  1         0.001             0.001        varies             0.05    0.05    0.4        0.4        1                 1  

var_bias2<-read.delim("sims_output_bias2.csv", sep=" ", header=T)

plot_bias2 <- ggplot(var_bias2) + 
  geom_point(aes(x=biast.,y=mean_t,colour="t")) + 
  geom_point(aes(x=biast.,y=mean_p,colour="p"))
plot_bias2

#Five replicates all in one
var_biasbulk<-read.delim("sims_output_biasbulk.csv", sep=" ", header=T)

plot_biasbulk <- ggplot(var_biasbulk) + 
  geom_point(aes(x=biast.,y=mean_t,colour="t")) + 
  geom_point(aes(x=biast.,y=mean_p,colour="p"))
plot_biasbulk

#   a       b                  c          biast             mu_p    mu_t    sdmu_p    sdmu_t   sex-limited p       sex-limited t
#  0         0.001          varies            0.01             0.05    0.05    0.4        0.4        1                 1  

#Five replicates all in one
var_a<-read.delim("sims_output_a.csv", sep=" ", header=T)

plot_a <- ggplot(var_a) + 
  geom_point(aes(x=c.,y=mean_p,colour="p")) + 
  geom_point(aes(x=c.,y=mean_t,colour="t"))
plot_a

#   a       b                  c          biast             mu_p    mu_t    sdmu_p    sdmu_t   sex-limited p       sex-limited t
#  1         0.0125          0.5         varies             0.05    0.05    0.05        0.05        1                 1  

pom<-read.delim("sims_output_pom.csv", sep=" ", header=T)

plot_pom <- ggplot(pom) + 
  geom_point(aes(x=biast.,y=mean_t,colour="t")) + 
  geom_point(aes(x=biast.,y=mean_p,colour="p"))
plot_pom

pomlil<-read.delim("sims_output_pomlil.csv", sep=" ", header=T)

plot_pomlil <- ggplot(pomlil) + 
  geom_point(aes(x=biast.,y=mean_t,colour="t")) + 
  geom_point(aes(x=biast.,y=mean_p,colour="p"))
plot_pomlil

#TRY PICKING A REASONABLE BIAS VALUE AND VARYING C AROUND IT - 0.5 MIGHT BE TOO BIG


#   a       b                  c          biast             mu_p    mu_t    sdmu_p    sdmu_t   sex-limited p       sex-limited t
#  1         0.0125          0.1         varies             0.05    0.05    0.05        0.05        1                 1  

pom2<-read.delim("sims_output_pom2.csv", sep=" ", header=T)

plot_pom2 <- ggplot(pom2) + 
  geom_point(aes(x=biast.,y=mean_t,colour="t")) + 
  geom_point(aes(x=biast.,y=mean_p,colour="p"))
plot_pom2

#   a       b                  c          biast             mu_p    mu_t    sdmu_p    sdmu_t   sex-limited p       sex-limited t
#  1         0.0125          0.05         varies             0.05    0.05    0.05        0.05        1                 1  

pom3<-read.delim("sims_output_pom3.csv", sep=" ", header=T)

plot_pom3 <- ggplot(pom3) + 
  geom_point(aes(x=biast.,y=mean_t,colour="t")) + 
  geom_point(aes(x=biast.,y=mean_p,colour="p"))
plot_pom3


# Original pom values

pompom<-read.delim("sims_output_pompom.csv", sep=" ", header=T)
plot_pompom <- ggplot(pompom) + 
  geom_point(aes(x=file,y=mean_t,colour="t")) + 
  geom_point(aes(x=file,y=mean_p,colour="p"))
plot_pompom

#Original pom values starting with p=3

pomp<-read.delim("sims_output_pomp.csv", sep=" ", header=T)
plot_pomp <- ggplot(pomp) + 
  geom_point(aes(x=file,y=mean_t,colour="t")) + 
  geom_point(aes(x=file,y=mean_p,colour="p"))
plot_pomp

#   a       b                  c          biast             mu_p    mu_t    sdmu_p    sdmu_t   sex-limited p       sex-limited t
#  1         0.0125          varies         0.005             0.05    0.05    0.05        0.05        1                 1  

pomc<-read.delim("sims_output_pom_c.csv", sep=" ", header=T)

plot_pomc <- ggplot(pomc) + 
  geom_point(aes(x=c.,y=mean_t,colour="t")) + 
  geom_point(aes(x=c.,y=mean_p,colour="p"))
plot_pomc

#   a       b                  c          biast             mu_p    mu_t    sdmu_p    sdmu_t   sex-limited p       sex-limited t
#  1         0.001            0.5         0.008             0.05    0.05    0.05        0.05        1                 1  
#Starting with p=3
#(from Fawcett paper)

faw<-read.delim("sims_output_faw.csv", sep=" ", header=T)
plot_faw <- ggplot(faw) + 
  geom_point(aes(x=file,y=mean_t,colour="t")) + 
  geom_point(aes(x=file,y=mean_p,colour="p"))
plot_faw

#New mutation function:
#   a       b                  c          biast             mu_p    mu_t    sdmu_p    sdmu_t   sex-limited p       sex-limited t
#  1         0.001            0.5         0.9              0.05    0.05    0.05        0.05        1                 1  
#Starting with p=3

mutfaw<-read.delim("sims_output_mutfaw.csv", sep=" ", header=T)
plot_mutfaw <- ggplot(mutfaw) + 
  geom_point(aes(x=file,y=mean_t,colour="t")) + 
  geom_point(aes(x=file,y=mean_p,colour="p"))
plot_mutfaw

#New monod Weber function:
#   a       b                  c          biast             mu_p    mu_t    sdmu_p    sdmu_t   sex-limited p       sex-limited t
#  1         0.001            varies         0.9              0.05    0.05    0.05        0.05        1                 1  
#Starting with p=3

monodWeb<-read.delim("sims_output_monodWeb.csv", sep=" ", header=T)
plot_monodWeb <- ggplot(monodWeb) + 
  geom_point(aes(x=c.,y=mean_t,colour="t")) + 
  geom_point(aes(x=c.,y=mean_p,colour="p"))
plot_monodWeb

#Compare with open-ended preference function:
#   a       b                  c          biast             mu_p    mu_t    sdmu_p    sdmu_t   sex-limited p       sex-limited t
#  1         0.001            varies         0.9              0.05    0.05    0.05        0.05        1                 1  
#Starting with p=3

monodfish<-read.delim("sims_output_monodfish.csv", sep=" ", header=T)
plot_monodfish <- ggplot(monodfish) + 
  geom_point(aes(x=c.,y=mean_t,colour="t")) + 
  geom_point(aes(x=c.,y=mean_p,colour="p"))
plot_monodfish
