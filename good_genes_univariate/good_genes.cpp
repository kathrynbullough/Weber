#include <cassert>
#include <stdexcept>
#include <vector>
#include <cmath>
#include <iostream>
#include "good_genes.hpp"
#include "parameters.hpp"

GoodGenes::GoodGenes(Parameters const &params) :
    par{params},
    data_file{par.file_name},
    uniform{0.0,1.0},
    rd{},
    seed{rd()},
    rng_r{seed},
    males(par.n/2,Individual(par)),
    females(par.n/2,Individual(par))
{
    write_data_headers();

    // set phenotypes for first generation
    phenotypes();

    for (time_step = 0; 
            time_step <= par.max_num_gen; ++time_step)
    {
        survival();
        reproduction();

        // move the expression of the male phenotype 
        // out of the survival function and put it in a separate
        // function, say, phenotypes()
        phenotypes();

        if (time_step % par.numoutgen == 0)
        {
            write_data();
        }
    }

    write_parameters();

} // end GoodGenes() constructor

void GoodGenes::phenotypes()
{
	double t[par.ntrait],v;
	for (auto male_iter{males.begin()}; male_iter !=males.end(); ++male_iter)
	{
		for (int trait_idx=0; trait_idx<par.ntrait; ++trait_idx)
		{
			t[trait_idx] = 0.5 * (male_iter->t[0][trait_idx] + male_iter->t[1][trait_idx]);
			v = 0.5 * (male_iter->v[0] + male_iter->v[1]);
			male_iter->x[trait_idx] = t[trait_idx] * std::exp(-std::fabs(par.v_opt - v));
		}
	}
}

void GoodGenes::survival()
{
    // aux variables to store trait values
    double p[par.ntrait],v,surv;

    unsigned nm = males.size();
    unsigned nf = females.size();

    mean_p_survive_f = 0.0;
    mean_p_survive_m = 0.0;
    
    for (auto female_iter{females.begin()}; female_iter != females.end(); )
    {
      surv = 0.0;
      for (int trait_idx = 0; trait_idx < par.ntrait; ++trait_idx)
	     {
        p[trait_idx] = 0.5 * (female_iter->p[0][trait_idx] + female_iter->p[1][trait_idx]);
        v = 0.5 * (female_iter->v[0] + female_iter->v[1]);

        surv = std::exp(-par.b * p[trait_idx] * p[trait_idx] - std::fabs(par.v_opt - v)) ;

        mean_p_survive_f += surv;
       }

        // individual dies
        if (uniform(rng_r) > surv)
        {
            std::swap(*female_iter, females.back()); // swap this female with final elmt
            females.pop_back(); // remove final elmt
                                // this is way faster than std::erase() 
        }
        else
        {
            ++female_iter;
        }
    } // end for females

    double x[par.ntrait];

    for (auto male_iter{males.begin()}; male_iter != males.end(); )
    {
      surv = 0.0;
      for (int trait_idx = 0; trait_idx < par.ntrait; ++trait_idx)
	     {
        v = 0.5 * (male_iter->v[0] + male_iter->v[1]);

        x[trait_idx] = male_iter->x[trait_idx];

        surv = std::exp(-par.c * x[trait_idx] * x[trait_idx] - std::fabs(par.v_opt - v)) ;
        
        mean_p_survive_m += surv;
       }

        // individual dies
        if (uniform(rng_r) > surv)
        {
            std::swap(*male_iter, males.back()); // swap this male with final elmt
            males.pop_back(); // remove final elmt
                                // this is way faster than std::erase() 
        }
        else
        {
            ++male_iter;
        }
    }

    mean_p_survive_f /= nf;
    mean_p_survive_m /= nm;
} // end survival


void GoodGenes::reproduction()
{
    long unsigned female_idx,male_idx;

    std::vector<Individual> nextgen{};

    std::uniform_int_distribution<unsigned> female_sampler(0, females.size() - 1);

    for (unsigned newborn_idx{0}; newborn_idx < par.n; ++newborn_idx)
    {
        female_idx = female_sampler(rng_r);

        assert(female_idx >= 0);
        assert(female_idx < females.size());

        male_idx = choose(females[female_idx]);
        
        assert(male_idx >= 0);
        assert(male_idx < males.size());
        
//        std::cout << male_idx << " " << males.size() << " " << female_idx << " " << females.size() << std::endl;

        Individual Kid(
                females[female_idx],
                males[male_idx],
                rng_r,
                par);

        nextgen.push_back(Kid);
    }

    assert(nextgen.size() == par.n);

    females.clear();
    males.clear();

    for (unsigned newborn_idx{0}; newborn_idx < par.n; ++newborn_idx)
    {
        if (uniform(rng_r) < 0.5)
        {
            males.push_back(nextgen[newborn_idx]);
        } else
        {
            females.push_back(nextgen[newborn_idx]);
        }
    }
} // end reproduction

void GoodGenes::write_parameters()
{
    data_file 
        << std::endl 
        << std::endl
        << "seed;" << seed << ";" << std::endl
        << "n;" << par.n << ";" << std::endl
        << "mu_p;" << par.mu_p << ";" << std::endl
        << "mu_t;" << par.mu_t << ";" << std::endl
        << "mu_v;" << par.mu_v << ";" << std::endl
        << "max_mut_p;" << par.max_mut_p << ";" << std::endl
        << "max_mut_t;" << par.max_mut_t << ";" << std::endl
        << "max_mut_v;" << par.max_mut_v << ";" << std::endl
        << "biasv;" << par.biasv << ";" << std::endl
        << "pref;" << par.pref << ";" << std::endl
        << "ntrait;" << par.ntrait << ";" << std::endl
        << "a;" << par.a << ";" << std::endl
        << "b;" << par.b << ";" << std::endl
        << "c;" << par.c << ";" << std::endl
        << "choice_sample_size;" << par.choice_sample_size << ";" << std::endl
        << "init_t;" << par.init_t << ";" << std::endl
        << "init_p;" << par.init_p << ";" << std::endl
        << "init_v;" << par.init_v << ";" << std::endl
        << "v_opt;" << par.v_opt << ";" << std::endl
        << "max_num_gen;" << par.max_num_gen << ";" << std::endl
        << "numoutgen;" << par.numoutgen << ";" << std::endl;
}

void GoodGenes::write_data()
{
    double meanp[par.ntrait];
    double ssp[par.ntrait];
    double meant[par.ntrait];
    double sst[par.ntrait];
    double meanv{0.0};
    double ssv{0.0};
    double meanx[par.ntrait];
    double ssx[par.ntrait];
    double varp[par.ntrait];
    double vart[par.ntrait];
    double varx[par.ntrait];
    double stv[par.ntrait];
    double stp[par.ntrait];
    double spv[par.ntrait];
    double covtp[par.ntrait];
    double covtv[par.ntrait];
    double covpv[par.ntrait];


    // keep track of population sizes
    unsigned long nf{females.size()};
    unsigned long nm{males.size()};

    // aux variables to store trait values
    double p[par.ntrait],t[par.ntrait],v,x[par.ntrait];

    for (auto female_iter{females.begin()};
            female_iter != females.end();
            ++female_iter)
    {
	v = 0.5 * (female_iter->v[0] + female_iter->v[1]);
	meanv += v;
	ssv += v*v;

      for (int trait_idx = 0; trait_idx < par.ntrait; ++trait_idx)
	     {
        p[trait_idx] = 0.5 * (female_iter->p[0][trait_idx] + female_iter->p[1][trait_idx]);
        meanp[trait_idx] += p[trait_idx];
        ssp[trait_idx] += p[trait_idx]*p[trait_idx];
        
        t[trait_idx] = 0.5 * (female_iter->t[0][trait_idx] + female_iter->t[1][trait_idx]);
        meant[trait_idx] += t[trait_idx];
        sst[trait_idx] += t[trait_idx]*t[trait_idx];

	stp[trait_idx] += t[trait_idx] * p[trait_idx];
	spv[trait_idx] += p[trait_idx] * v;
	stv[trait_idx] += t[trait_idx] * v;

       }

    }
    
    for (auto male_iter{males.begin()};
            male_iter != males.end();
            ++male_iter)
    {
        v = 0.5 * (male_iter->v[0] + male_iter->v[1]);
        meanv += v;
        ssv += v*v;

      for (int trait_idx = 0; trait_idx < par.ntrait; ++trait_idx)
	     {
        p[trait_idx] = 0.5 * (male_iter->p[0][trait_idx] + male_iter->p[1][trait_idx]);
        meanp[trait_idx] += p[trait_idx];
        ssp[trait_idx] += p[trait_idx]*p[trait_idx];
        
        t[trait_idx] = 0.5 * (male_iter->t[0][trait_idx] + male_iter->t[1][trait_idx]);
        meant[trait_idx] += t[trait_idx];
        sst[trait_idx] += t[trait_idx]*t[trait_idx];
        
        x[trait_idx] = male_iter->x[trait_idx];
        meanx[trait_idx] += x[trait_idx];
        ssx[trait_idx] += x[trait_idx]*x[trait_idx];
      
	stp[trait_idx] += t[trait_idx] * p[trait_idx];
	spv[trait_idx] += p[trait_idx] * v;
	stv[trait_idx] += t[trait_idx] * v;

	}
       
    }

    meanv /= (nf + nm);
    double varv = ssv / (nf + nm) - meanv * meanv;

  for (int trait_idx = 0; trait_idx < par.ntrait; ++trait_idx)
	     {
        meanp[trait_idx] /= (nf + nm);
        meant[trait_idx] /= (nf + nm);
        meanx[trait_idx] /= nm;
        varp[trait_idx] = ssp[trait_idx] / (nf + nm) - meanp[trait_idx] * meanp[trait_idx];
        vart[trait_idx] = sst[trait_idx] / (nf + nm) - meant[trait_idx] * meant[trait_idx];
        varx[trait_idx] = ssx[trait_idx] / nm - meanx[trait_idx] * meanx[trait_idx];
        covtp[trait_idx] = stp[trait_idx] / (nf + nm) - meant[trait_idx] * meanp[trait_idx];
        covtv[trait_idx] = stv[trait_idx] / (nf + nm) - meant[trait_idx] * meanv;
        covpv[trait_idx] = spv[trait_idx] / (nf + nm) - meanp[trait_idx] * meanv;
	     }

    data_file << time_step << ";";
    for (int trait_idx = 0; trait_idx < par.ntrait; ++trait_idx)
	     {
        data_file << meanp[trait_idx] << ";"
        << meant[trait_idx] << ";"
        << meanx[trait_idx] << ";"
        << varp[trait_idx] << ";"
        << vart[trait_idx] << ";"
        << varx[trait_idx] << ";"
	<< covtp[trait_idx] << ";"
	<< covtv[trait_idx] << ";"
	<< covpv[trait_idx] << ";";
       }
        data_file << meanv << ";"
        << varv << ";"
        << mean_p_survive_f << ";"
        << mean_p_survive_m << ";"
        << nf << ";"
        << nm << ";" << std::endl;
} // write_data()

void GoodGenes::write_data_headers()
{   
    data_file << "generation" << ";";
    for (int trait_idx = 0; trait_idx < par.ntrait; ++trait_idx)
    {
	    data_file 
            << "meanp" << (trait_idx + 1) << ";"
            << "meant" << (trait_idx + 1) << ";"
            << "meanx" << (trait_idx + 1) << ";"
            << "varp" << (trait_idx + 1) << ";"
            << "vart" << (trait_idx + 1) << ";"
            << "varx" << (trait_idx + 1) << ";"
	    << "covtp" << (trait_idx +1) << ";"
	    << "covtv" << (trait_idx +1) << ";"
	    << "covpv" << (trait_idx +1) << ";";
    }

    data_file << "meanv"
    << ";varv"
    << ";surv_f"
        << ";surv_m"
        << ";nf"
        << ";nm;"
	<< std::endl;
}

// choose surviving male according to its ornament 
unsigned GoodGenes::choose(Individual const &female)
{
    std::vector <double> male_fitness{};
    std::vector <unsigned> male_idxs{};

    // distribution to sample males from
    std::uniform_int_distribution<unsigned> 
        male_sampler(0, males.size() - 1);

    unsigned sampled_male_idx;
    
    double fitness;

    double p[par.ntrait];
    p[par.ntrait] = 0.5 * (female.p[0][par.ntrait] + female.p[1][par.ntrait]);

    double x[par.ntrait];
    
    switch(par.pref)
    {
        //Open-ended preferences
        case 0:
            {
                for (unsigned inspected_male_idx{0}; 
                        inspected_male_idx < par.choice_sample_size; 
                        ++inspected_male_idx)
                {
                  for (int trait_idx  = 0; trait_idx < par.ntrait; ++trait_idx)
                  {
                    sampled_male_idx = male_sampler(rng_r);

                    x[trait_idx] = males[sampled_male_idx].x[trait_idx];

                    fitness = std::exp(par.a * p[trait_idx] * x[trait_idx]);
                  }
                    
                    male_idxs.push_back(sampled_male_idx);
                    male_fitness.push_back(fitness);
                }
            }
            break;
    
    //Weber preferences
        case 1:
            {
                for (unsigned inspected_male_idx{0}; 
                        inspected_male_idx < par.choice_sample_size; 
                        ++inspected_male_idx)
                {
                  for (int trait_idx  = 0; trait_idx < par.ntrait; ++trait_idx)
                  {
                    sampled_male_idx = male_sampler(rng_r);

                    x[trait_idx] = males[sampled_male_idx].x[trait_idx];

                    fitness = par.a * (x[trait_idx] / (x[trait_idx] + p[trait_idx]));
                  }
                    
                    male_idxs.push_back(sampled_male_idx);
                    male_fitness.push_back(fitness);
                }
            
            } 
            break;

        default:
            std::cout << "Something's gone wrong!";
            break;
    }

    // now make distribution of the fitnesses to choose from
    std::discrete_distribution<unsigned> choose_male(male_fitness.begin(),
            male_fitness.end());

    unsigned the_chosen_male = male_idxs[choose_male(rng_r)];

    return(the_chosen_male);
} // choose()



