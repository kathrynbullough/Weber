#ifndef _INDIVIDUAL_HPP_
#define _INDIVIDUAL_HPP_

#include <random>
#include "parameters.hpp"

class Individual
{
    public:
        std::vector<double> t[2];
        std::vector<double> p[2];
        
        // realized ornament
        std::vector<double> x;
        
        double v[2]{0.0,0.0};

        // male survival prob

        // standard constructor
        Individual(Parameters const &param);

        // copy ctor
        Individual(Individual const &other);

        // birth constructor
        Individual(Individual const &mother,
                Individual const&father,
                std::mt19937 &rng_r,
                Parameters const &params);

        void operator=(Individual const &other);
};

#endif 
