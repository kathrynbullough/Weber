#ifndef _GOOD_GENES_HPP_
#define _GOOD_GENES_HPP_

#include <vector>
#include <random>

#include <fstream>
#include "individual.hpp"

class GoodGenes
{
    private:
        Parameters par;

        std::ofstream data_file;
        std::uniform_real_distribution<double> uniform;

        std::random_device rd;
        unsigned seed;
        std::mt19937 rng_r;

        unsigned time_step{0};

        double mean_p_survive_f{0.0};
        double mean_p_survive_m{0.0};

        std::vector<Individual> males;
        std::vector<Individual> females;

        void reproduction();
        void survival();
        unsigned choose(Individual const &female);

        void write_data();
        void write_data_headers();
        void write_parameters();

    public:
        GoodGenes(Parameters const &parameters);



};


#endif
