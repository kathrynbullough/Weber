#include <iostream>
#include <cmath>
#include <cassert>
#include "individual.hpp"
#include "parameters.hpp"

// standard constructor
Individual::Individual(Parameters const &params) :
    t{std::vector<double>(params.ntrait,params.init_t),
      std::vector<double>(params.ntrait,params.init_t)},
    p{std::vector<double>(params.ntrait,params.init_p),
      std::vector<double>(params.ntrait,params.init_p)},
    x{std::vector<double>(params.ntrait,(params.init_t*std::exp(-std::fabs(params.v_opt - params.init_v))))},
    v{params.init_v, params.init_v}
{
}

Individual::Individual(Individual const &other) :
    t{other.t[0], other.t[1]},
    p{other.p[0], other.p[1]},
    x{other.x},
    v{other.v[0], other.v[1]}
{
}


Individual::Individual(
                Individual const &mother,
                Individual const &father,
                std::mt19937 &rng_r,
                Parameters const &params) :
    t{std::vector<double>(params.ntrait,0.0),
      std::vector<double>(params.ntrait,0.0)},
    p{std::vector<double>(params.ntrait,0.0),
      std::vector<double>(params.ntrait,0.0)},
    x{std::vector<double>(params.ntrait,0.0)}               
{

    std::bernoulli_distribution segregator{0.5};
    std::uniform_real_distribution<double> uniform{0.0,1.0};
    
   for (unsigned trait_idx = 0; trait_idx < params.ntrait; ++trait_idx)
	   {
      p[0][trait_idx] = mother.p[segregator(rng_r)][trait_idx];
      p[1][trait_idx] = father.p[segregator(rng_r)][trait_idx];

      t[0][trait_idx] = mother.t[segregator(rng_r)][trait_idx];
      t[1][trait_idx] = father.t[segregator(rng_r)][trait_idx];
     }

    v[0] = mother.v[segregator(rng_r)];
    v[1] = father.v[segregator(rng_r)];

    double incr;
  
    for (unsigned allele_idx{0}; allele_idx < 2; ++allele_idx)
    {
      for (unsigned trait_idx = 0; trait_idx < params.ntrait; ++trait_idx)
	     {
        if (uniform(rng_r) < params.mu_p)
        {
            incr = uniform(rng_r) * params.max_mut_p;
            p[allele_idx][trait_idx] = p[allele_idx][trait_idx] + 
                (uniform(rng_r) < 0.5 ? -incr : incr);
           
        }

        if (uniform(rng_r) < params.mu_t)
        {
            incr = uniform(rng_r) * params.max_mut_t;
            t[allele_idx][trait_idx] = t[allele_idx][trait_idx] + 
                (uniform(rng_r) < 0.5 ? -incr : incr);
            
        }
       }
        
        if (uniform(rng_r) < params.mu_v)
        {
            incr = uniform(rng_r) * params.max_mut_v;
            v[allele_idx] = v[allele_idx] + 
                (uniform(rng_r) < params.biasv ? -incr : incr);
        }
    } // end for allele_idx
} // end Individual()

void Individual::operator=(Individual const &other)
{
    for (unsigned allele_idx{0}; allele_idx < 2; ++allele_idx)
    {
        assert(other.t[0].size() == other.t[1].size());
        assert(other.p[0].size() == other.p[1].size());

      for (unsigned trait_idx = 0; trait_idx < other.t[0].size(); ++trait_idx)
	     {
        p[allele_idx][trait_idx] = other.p[allele_idx][trait_idx];
        t[allele_idx][trait_idx] = other.t[allele_idx][trait_idx];
       }
        v[allele_idx] = other.v[allele_idx];
    }

    assert(other.p[0].size() == other.x.size());
    assert(x.size() == other.x.size());

    for (unsigned trait_idx = 0; trait_idx < other.x.size(); ++trait_idx)
    {
        x[trait_idx] = other.x[trait_idx];
    }

}//end operator=
//
