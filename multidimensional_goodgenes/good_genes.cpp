#include <cassert>
#include <stdexcept>
#include <vector>
#include <cmath>
#include <array>
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
        // individuals survive...
        survival();
        
        // ... then reproduce
        reproduction();

        // then next generation expresses its phenotypes (x)
        phenotypes();

        if (time_step % par.numoutgen == 0)
        {
            write_data();
        }
    }

    // then finally after everything is  over,
    // write parameters to output file
    write_parameters();

} // end GoodGenes() constructor

void GoodGenes::phenotypes()
{
	double t, v;


	for (auto male_iter{males.begin()}; male_iter !=males.end(); ++male_iter)
	{
        v = 0.5 * (male_iter->v[0] + male_iter->v[1]);

		for (unsigned trait_idx = 0; trait_idx < par.ntrait; ++trait_idx)
		{
			t = 0.5 * (male_iter->t[0][trait_idx] + male_iter->t[1][trait_idx]);

			male_iter->x[trait_idx] = t * std::exp(-std::fabs(par.v_opt - v));
		}
	}
}

void GoodGenes::survival()
{
    // aux variables to store trait values
    double p,v,sum_surv,sum_p;

    unsigned nm = males.size();
    unsigned nf = females.size();

    mean_p_survive_f = 0.0;
    mean_p_survive_m = 0.0;

    // calculate theta inverse
    double thet_inv = 1.0/par.thet;

    std::vector <double> survival_distribution;
    
    for (auto female_iter{females.begin()}; female_iter != females.end(); ++female_iter)
    {
      sum_surv = 0.0;
      sum_p = 0.0;
        
      v = 0.5 * (female_iter->v[0] + female_iter->v[1]);

        for (unsigned trait_idx = 0; trait_idx < par.ntrait; ++trait_idx)
        {
            p = 0.5 * (female_iter->p[0][trait_idx] + female_iter->p[1][trait_idx]);

            sum_p += std::fabs(pow(par.lambda * p,thet_inv));
       }
        
        sum_surv = std::exp(-par.b *pow(sum_p,(par.gam*par.thet))-std::fabs(par.v_opt - v));

        survival_distribution.push_back(sum_surv);

        mean_p_survive_f += sum_surv;
    }

    // make survival distribution
    std::discrete_distribution<unsigned> female_survival_distribution(
            survival_distribution.begin(),
            survival_distribution.end());

    unsigned max_survivors_per_sex = par.fraction_survivors * 
        survival_distribution.size();

    std::vector <Individual> surviving_females;

    for (unsigned surviving_female_idx{0};
            surviving_female_idx < max_survivors_per_sex;
            ++surviving_female_idx)
    {
        surviving_females.push_back(
                females[female_survival_distribution(rng_r)]
                );

    } // end for females

    females = surviving_females;

    survival_distribution.clear();

    double x,sum_cxx;

    for (auto male_iter{males.begin()}; male_iter != males.end(); ++male_iter)
    {
      sum_surv = 0.0;
      sum_cxx = 0.0;
        
      v = 0.5 * (male_iter->v[0] + male_iter->v[1]);
      
        for (unsigned trait_idx = 0; trait_idx < par.ntrait; ++trait_idx)
        {
            x = male_iter->x[trait_idx];

            sum_cxx += (par.c/(1.0 + par.k*v))*x*x;
       }

        sum_surv = std::exp(-sum_cxx - std::fabs(par.v_opt - v));

        mean_p_survive_m += sum_surv;

        survival_distribution.push_back(sum_surv);
    }
    

    // make survival distribution
    std::discrete_distribution<unsigned> male_survival_distribution(
            survival_distribution.begin(),
            survival_distribution.end());

    max_survivors_per_sex = par.fraction_survivors * 
        survival_distribution.size();

   std::vector <Individual> surviving_males;

    for (unsigned surviving_male_idx{0};
            surviving_male_idx < max_survivors_per_sex;
            ++surviving_male_idx)
    {
        surviving_males.push_back(
                males[male_survival_distribution(rng_r)]
                );

    } // end for females

    males = surviving_males;

     //Change these to sum_surv instead of mean_p_surv??
    mean_p_survive_f /= nf;
    mean_p_survive_m /= nm;
} // end survival


void GoodGenes::reproduction()
{
    if (females.size() < 1 || males.size() < 1)
    {
        write_data();
        write_parameters();
        exit(1);
    }


    long unsigned female_idx,male_idx;

    // reset stats that keeps track of the value of the preference function
    // of the male that actually gets chosen
    sampled_preffunc = 0.0;

    std::vector<Individual> nextgen{};

    std::uniform_int_distribution<unsigned> female_sampler(0, females.size() - 1);

    for (unsigned newborn_idx{0}; newborn_idx < par.n; ++newborn_idx)
    {
        female_idx = female_sampler(rng_r);

        male_idx = choose(females[female_idx]);
        
        Individual Kid(
                females[female_idx],
                males[male_idx],
                rng_r,
                par);

        nextgen.push_back(Kid);
    }

    sampled_preffunc /= par.n;

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
        << "fraction_survivors;" << par.fraction_survivors << ";" << std::endl
        << "a;" << par.a << ";" << std::endl
        << "b;" << par.b << ";" << std::endl
        << "c;" << par.c << ";" << std::endl
	<< "gam;" << par.gam << ";" << std::endl
	<< "thet;" << par.thet << ";" << std::endl
        << "choice_sample_size;" << par.choice_sample_size << ";" << std::endl
        << "init_t;" << par.init_t << ";" << std::endl
        << "init_p;" << par.init_p << ";" << std::endl
        << "init_v;" << par.init_v << ";" << std::endl
        << "weber_k;" << par.weber_k << ";" << std::endl
        << "v_opt;" << par.v_opt << ";" << std::endl
        << "max_num_gen;" << par.max_num_gen << ";" << std::endl
        << "numoutgen;" << par.numoutgen << ";" << std::endl;
}

void GoodGenes::write_data()
{
    std::vector <double> meanp(par.ntrait,0.0);
    std::vector <double> ssp(par.ntrait,0.0);
    std::vector <double> meant(par.ntrait,0.0);
    std::vector <double> sst(par.ntrait,0.0);
    double meanv{0.0};
    double ssv{0.0};
    std::vector <double> meanx(par.ntrait,0.0);
    std::vector <double> ssx(par.ntrait,0.0);
    std::vector <double> varp(par.ntrait,0.0);
    std::vector <double> vart(par.ntrait,0.0);
    std::vector <double> varx(par.ntrait,0.0);
    std::vector <double> stv(par.ntrait,0.0);
    std::vector <double> stp(par.ntrait,0.0);
    std::vector <double> spv(par.ntrait,0.0);
    std::vector <double> covtp(par.ntrait,0.0);
    std::vector <double> covtv(par.ntrait,0.0);
    std::vector <double> covpv(par.ntrait,0.0);


    // keep track of population sizes
    unsigned long nf{females.size()};
    unsigned long nm{males.size()};

    // aux variables to store trait values
    double v, p, t, x;

    for (auto female_iter{females.begin()};
            female_iter != females.end();
            ++female_iter)
    {
        v = 0.5 * (female_iter->v[0] + female_iter->v[1]);
        meanv += v;
        ssv += v*v;

        for (unsigned trait_idx = 0; trait_idx < par.ntrait; ++trait_idx)
        {
            p = 0.5 * (female_iter->p[0][trait_idx] + female_iter->p[1][trait_idx]);
            meanp[trait_idx] += p;
            ssp[trait_idx] += p*p;
        
            t = 0.5 * (female_iter->t[0][trait_idx] + female_iter->t[1][trait_idx]);
            meant[trait_idx] += t;
            sst[trait_idx] += t*t;

            stp[trait_idx] += t * p;
            spv[trait_idx] += p * v;
            stv[trait_idx] += t * v;
        }
    }
    
    for (auto male_iter{males.begin()};
            male_iter != males.end();
            ++male_iter)
    {
        v = 0.5 * (male_iter->v[0] + male_iter->v[1]);
        meanv += v;
        ssv += v*v;

          for (unsigned trait_idx = 0; trait_idx < par.ntrait; ++trait_idx)
         {
            p = 0.5 * (male_iter->p[0][trait_idx] + male_iter->p[1][trait_idx]);
            meanp[trait_idx] += p;
            ssp[trait_idx] += p*p;
            
            t = 0.5 * (male_iter->t[0][trait_idx] + male_iter->t[1][trait_idx]);
            meant[trait_idx] += t;
            sst[trait_idx] += t*t;
            
            x = male_iter->x[trait_idx];
            meanx[trait_idx] += x;
            ssx[trait_idx] += x*x;
          
            stp[trait_idx] += t * p;
            spv[trait_idx] += p * v;
            stv[trait_idx] += t * v;
        }
       
    }

    meanv /= (nf + nm);
    double varv = ssv / (nf + nm) - meanv * meanv;

    for (unsigned trait_idx = 0; trait_idx < par.ntrait; ++trait_idx)
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
    for (unsigned trait_idx = 0; trait_idx < par.ntrait; ++trait_idx)
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
        << sampled_preffunc << ";"
        << nf << ";"
        << nm << ";" << std::endl;
} // write_data()

void GoodGenes::write_data_headers()
{   
    data_file << "generation" << ";";
    for (unsigned trait_idx = 0; trait_idx < par.ntrait; ++trait_idx)
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
        << ";preffunc"
        << ";nf"
        << ";nm;"
	<< std::endl;
}

// choose surviving male according to its ornament 
unsigned GoodGenes::choose(Individual const &female)
{
    std::vector <double> male_fitness{};
    std::vector <unsigned> male_idxs{};

    // uniform distribution to sample males from
    std::uniform_int_distribution<unsigned> 
        male_sampler(0, males.size() - 1);

    unsigned sampled_male_idx;

    double x, sum_fitness;

    std::vector <double> p(par.ntrait, 0.0);

    for (unsigned trait_idx = 0; trait_idx < par.ntrait; ++trait_idx)
    {
        p[trait_idx] = 0.5 * (female.p[0][trait_idx] + female.p[1][trait_idx]);
    }
    
    switch(par.pref)
    {
        //Open-ended preferences
        case 0:
        {
            for (unsigned inspected_male_idx{0}; 
                    inspected_male_idx < par.choice_sample_size; 
                    ++inspected_male_idx)
            {
                sum_fitness = 0.0;
                
                sampled_male_idx = male_sampler(rng_r);

                for (unsigned trait_idx  = 0; trait_idx < par.ntrait; ++trait_idx)
                {
                    x = males[sampled_male_idx].x[trait_idx];

                    sum_fitness += par.a * p[trait_idx] * x;
                }

                sum_fitness = std::exp(sum_fitness);

                assert(std::isnormal(sum_fitness));

                male_idxs.push_back(sampled_male_idx);
                male_fitness.push_back(sum_fitness);
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
                sum_fitness = 0.0;
                
                sampled_male_idx = male_sampler(rng_r);

                for (unsigned trait_idx  = 0; trait_idx < par.ntrait; ++trait_idx)
                {
                    x = males[sampled_male_idx].x[trait_idx] + 
                            males[sampled_male_idx].x[trait_idx] * par.weber_k * 
                                standard_normal(rng_r);

                    sum_fitness += par.a * p[trait_idx] * x;
                }

                sum_fitness = std::exp(sum_fitness);

                assert(std::isnormal(sum_fitness));

                male_idxs.push_back(sampled_male_idx);
                male_fitness.push_back(sum_fitness);
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

    // keep track of the value of the preference function
    sampled_preffunc += male_fitness[the_chosen_male];

    return(the_chosen_male);
} // choose()



