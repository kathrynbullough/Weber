#include "individual.hpp"
#include "parameters.hpp"

// standard constructor
Individual::Individual(Parameters const &params) :
    t{params.init_t, params.init_t}[params.ntrait],
    p{params.init_p, params.init_p}[params.ntrait],
    v{params.init_v, params.init_v},
    x{0.0}[params.ntrait]
{
}

Individual::Individual(Individual const &other) :
    t{other.t[0], other.t[1]}[other.ntrait],
    p{other.p[0], other.p[1]}[other.ntrait],
    v{other.v[0], other.v[1]},
    x{other.x}[other.ntrait]
{}


Individual::Individual(
                Individual const &mother,
                Individual const &father,
                std::mt19937 &rng_r,
                Parameters const &params)
{
    std::bernoulli_distribution segregator{0.5};
    std::uniform_real_distribution<double> uniform{0.0,1.0};
    
   for (int trait_idx = 0; trait_idx < params.ntrait; ++trait_idx)
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
      for (int trait_idx = 0; trait_idx < params.ntrait; ++trait_idx)
	     {
        if (uniform(rng_r) < params.mu_p[trait_idx])
        {
            incr = uniform(rng_r) * params.max_mut_p;

            p[allele_idx][trait_idx] = p[allele_idx][trait_idx] + (uniform(rng_r) < 0.5 ? -incr : incr);
            
            if (p[allele_idx][trait_idx] < 0)
            {
                p[allele_idx][trait_idx] = 0;
            }
        }

        if (uniform(rng_r) < params.mu_t[trait_idx])
        {
            incr = uniform(rng_r) * params.max_mut_t;
            t[allele_idx][trait_idx] = t[allele_idx][trait_idx] + (uniform(rng_r) < 0.5 ? -incr : incr);
            
            if (t[allele_idx][trait_idx] < 0)
            {
                t[allele_idx][trait_idx] = 0;
            }
        }
       }
        
        if (uniform(rng_r) < params.mu_v)
        {
            incr = uniform(rng_r) * params.max_mut_v;
            v[allele_idx] = v[allele_idx] + (uniform(rng_r) < params.biasv ? -incr : incr);
        }
    } // end for allele_idx
} // end Individual()

void Individual::operator=(Individual const &other)
{
    for (unsigned allele_idx{0}; allele_idx < 2; ++allele_idx)
    {
      for (int trait_idx = 0; trait_idx < other.ntrait; ++trait_idx)
	     {
        p[allele_idx][trait_idx] = other.p[allele_idx][trait_idx];
        t[allele_idx][trait_idx] = other.t[allele_idx][trait_idx];
       }
        v[allele_idx] = other.v[allele_idx];
    }
}//end operator=
