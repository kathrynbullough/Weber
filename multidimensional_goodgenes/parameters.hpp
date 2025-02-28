#ifndef _PARAMETERS_HPP_
#define _PARAMETERS_HPP_

#include <string>

class Parameters
{
    public:
        unsigned n{5000};
        double mu_p{0.05};
        double mu_t{0.05};
        double mu_v{0.05};
        double max_mut_p{0.4};
        double max_mut_t{0.4};
        double max_mut_v{0.8};
        double biasv{0.99};

        double a{1.0};
        double b{0.0025};
        double c{0.5};
        double weber_k{0.0};

        double baseline_survival{0.0};
        
        int pref{0};
        unsigned ntrait{1};

        unsigned choice_sample_size{10};

        double init_t{2.0};
        double init_p{3.0};
        double init_v{5.0};
        double v_opt{10.0};

	double lambda{1.0};
	double gam{2.0};
	double thet{1.0};
	double k{0.0};

        unsigned max_num_gen{1000};
        unsigned numoutgen{10};

        std::string file_name{"sim_good_genes"};
};
#endif
