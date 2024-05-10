library("tidyverse")

the_data <- read_delim("summary.csv",delim=";")

the_data_l <- pivot_longer(the_data
                           ,cols=c(meant,meanp)
                           ,names_to = "trait"
                           ,values_to = "trait_value")

ggplot(data = the_data_l
       ,mapping = aes(x = biasv, y = trait_value)) +
    geom_point(mapping=aes(colour=trait)) +
    scale_colour_brewer(palette="Set1") +
    theme_classic() +
    xlab("Probability of biased mutations on viability trait") +
    ylab("Trait value")


ggsave(filename="overview.pdf",width=5,height=5,device = cairo_pdf)