#!/usr/bin/env Rscript
# plots everything from a single sim 

library("tidyverse")
library("patchwork")

# obtain the final line of the actual simulation data
# of each file
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

plot_mean_t <- ggplot(data=sim_data
        ,mappping=aes(x=generation
                ,y=mean_t)) + 
    geom_line(mapping=aes(x=generation,y=mean_t))

## plot mean p in a separate graph
plot_mean_p <- ggplot(data=sim_data
        ,mappping=aes(x=generation
                ,y=mean_p)) + geom_line(mapping=aes(x=generation,y=mean_p))

plot_mean_t / plot_mean_p

ggsave(file=paste0("graph_",basename(file.name),".pdf"))
    
stop()


plot(data$generation,data$mean_t)
plot(data$generation,data$mean_p)
plot(data$generation,data$surviving_males)
plot(data$meanmrs,data$mean_p)
plot(data$generation,data$covpt)
plot(data$mean_t,data$mean_p)

mod1<-glm(data$meanmrs~data$mean_t*data$mean_p*data$covpt)
summary(mod1)
abline(mod1)


data2 <- read.delim("output_fisher_TEST2.csv", sep=";", header=T)
plot(data2$generation,data2$mean_t)
plot(data2$generation,data2$mean_p)
plot(data2$generation,data2$surviving_males)
plot(data2$meanmrs,data2$mean_t)
plot(data2$mean_t,data2$mean_p,type="l")

mod2<-glm(data2$meanmrs~data2$mean_t)
summary(mod2)

data3 <- read.delim("output_fisher_nopcost.csv", sep=";", header=T)
plot(data3$generation,data3$mean_t)
plot(data3$generation,data3$mean_p)
plot(data3$meanmrs,data3$mean_t)
plot(data3$mean_t,data3$mean_p,type="l")

mod3<-glm(data3$meanmrs~data3$mean_t)
summary(mod3)

data4 <- read.delim("output_fisher_noptcost.csv", sep=";", header=T)
plot(data4$mean_t,data4$mean_p)
plot(data4$generation,data4$mean_t)
plot(data4$generation,data4$mean_p)

data5 <- read.delim("output_fisher_noptcost2.csv", sep=";", header=T)
plot(data5$mean_t,data5$mean_p,type="l",pch=18)
plot(data5$generation,data5$mean_t)
plot(data5$generation,data5$mean_p)
plot(data5$meanmrs,data5$mean_t)

web <- read.delim("output_fisher_Webtestyay.csv", sep=";", header=T)
plot(web$mean_t,web$mean_p,type="l",pch=18)
plot(web$generation,web$mean_t,type="l",pch=18)
plot(web$generation,web$mean_p,type="l",pch=18)
plot(web$meanmrs,web$mean_t)

web2 <- read.delim("output_fisher_Webtest2.csv", sep=";", header=T)
plot(web2$mean_t,web2$mean_p,type="l",pch=18)
plot(web2$generation,web2$mean_t,type="l",pch=18)
plot(web2$generation,web2$mean_p,type="l",pch=18)
plot(web2$meanmrs,web2$mean_t)

web3 <- read.delim("output_fisher_costlyWeb.csv", sep=";", header=T)
plot(web3$mean_t,web3$mean_p,type="l",pch=18)
plot(web3$generation,web3$mean_t,type="l",pch=18)
plot(web3$generation,web3$mean_p,type="l",pch=18)
plot(web3$meanmrs,web3$mean_t)
plot(web3$generation,web3$covpt,type="l",pch=18)

web4 <- read.delim("test.csv", sep=";", header=T)
plot(web3$mean_t,web3$mean_p,type="l",pch=18)
plot(web4$generation,web4$mean_t,type="l",pch=18)
plot(web4$generation,web4$mean_p,type="l",pch=18)
plot(web3$meanmrs,web3$mean_t)
plot(web4$generation,web4$covpt,type="l",pch=18)




par(mfrow=c(5,1))

test1 <- read.delim("output_fisher1.csv", sep=";", header=T)
test2 <- read.delim("output_fisher2.csv", sep=";", header=T)
test3 <- read.delim("output_fisher3.csv", sep=";", header=T)
test4 <- read.delim("output_fisher4.csv", sep=";", header=T)
test5 <- read.delim("output_fisher5.csv", sep=";", header=T)

plot(test1$generation,test1$mean_t,type="l",pch=18)
plot(test2$generation,test2$mean_t,type="l",pch=18)
plot(test3$generation,test3$mean_t,type="l",pch=18)
plot(test4$generation,test4$mean_t,type="l",pch=18)
plot(test5$generation,test5$mean_t,type="l",pch=18)

plot(test1$generation,test1$mean_p,type="l",pch=18)
plot(test2$generation,test2$mean_p,type="l",pch=18)
plot(test3$generation,test3$mean_p,type="l",pch=18)
plot(test4$generation,test4$mean_p,type="l",pch=18)
plot(test5$generation,test5$mean_p,type="l",pch=18)

null1 <- read.delim("null1.csv", sep=";", header=T)
null2 <- read.delim("null2.csv", sep=";", header=T)
null3 <- read.delim("null3.csv", sep=";", header=T)
null4 <- read.delim("null4.csv", sep=";", header=T)
null5 <- read.delim("null5.csv", sep=";", header=T)

plot(null1$generation,null1$mean_t,type="l",pch=18)
plot(null2$generation,null2$mean_t,type="l",pch=18)
plot(null3$generation,null3$mean_t,type="l",pch=18)
plot(null4$generation,null4$mean_t,type="l",pch=18)
plot(null5$generation,null5$mean_t,type="l",pch=18)

plot(null1$generation,null1$mean_p,type="l",pch=18)
plot(null2$generation,null2$mean_p,type="l",pch=18)
plot(null3$generation,null3$mean_p,type="l",pch=18)
plot(null4$generation,null4$mean_p,type="l",pch=18)
plot(null5$generation,null5$mean_p,type="l",pch=18)

test11 <- read.delim("output_fisher11.csv", sep=";", header=T)
test22 <- read.delim("output_fisher22.csv", sep=";", header=T)
test33 <- read.delim("output_fisher33.csv", sep=";", header=T)
test44 <- read.delim("output_fisher44.csv", sep=";", header=T)
test55 <- read.delim("output_fisher55.csv", sep=";", header=T)

plot(test11$generation,test11$mean_t,type="l",pch=18)
plot(test22$generation,test22$mean_t,type="l",pch=18)
plot(test33$generation,test33$mean_t,type="l",pch=18)
plot(test44$generation,test44$mean_t,type="l",pch=18)
plot(test55$generation,test55$mean_t,type="l",pch=18)

plot(test11$generation,test11$mean_p,type="l",pch=18)
plot(test22$generation,test22$mean_p,type="l",pch=18)
plot(test33$generation,test33$mean_p,type="l",pch=18)
plot(test44$generation,test44$mean_p,type="l",pch=18)
plot(test55$generation,test55$mean_p,type="l",pch=18)


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


