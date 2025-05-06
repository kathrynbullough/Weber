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

        double fraction_survivors{0.5};
        
        int pref{0};
        unsigned ntrait{1};

        unsigned choice_sample_size{10};

        double init_t{0.0};
        double init_p{1.0};
        double init_v{5.0};
        double v_opt{10.0};

	double lambda{1.0}; // weighting coefficient of each trait
	double gam{2.0}; // power with which survival drops away 
                     // away from a naturally selected optimal preference value of
                     // p_i = 0
	double thet{1.0}; // multiplicativity parameter of how cost scales with 
                      // number of preferences. Theta is low: use of multiple
                      // preferences has small costs. Theta is high: use of 
                      // multiple preferences has high costs
	double k{0.0}; // variable that scales how costs of ornaments depend on condition
                   // k = 0: all individuals have the same cost coefficient regardless
                   // of quality. k > 0: individuals of higher quality pay a smaller cost

        unsigned max_num_gen{1000};
        unsigned numoutgen{10};

        std::string file_name{"sim_good_genes"};
};
#endif
