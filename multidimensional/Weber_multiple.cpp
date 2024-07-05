//      Fisherian sexual selection for gonochorists with addition of a Weber preference function

//KATHRYN EDIT TEST


// load some libraries that we need
#include <ctime>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <cmath>
#include <cassert>
#include <random>

// random number generators
//
// we need something to 'seed' our random
// number generators (i.e., which sequence 
// of random numbers will we pick)
std::random_device rd; 
unsigned seed = rd();

// the actual random number generator
std::mt19937 rng_r(seed);

// allocate a uniform [0,1] random number distribution
std::uniform_real_distribution <double> uniform(0.0,1.0);
std::uniform_real_distribution <double> uniform_mutation(0.0,0.4);
std::bernoulli_distribution segregator(0.5);



// parameters (some of these values are changed upon initialization
// through the command line, see the argv and argc arrays in the 
// init function down below)
const int N = 5000; // population size
const int N_mate_sample = 10; // number of mates sampled
const int clutch_size = 10; // number of offspring produced

const int ntrait_max = 10;
int ntrait = 2;

double init_t[ntrait_max]; // initial value for ornament
double init_p[ntrait_max]; // initial value for preference
double a = 1.0; // choice slope
double b = 0.5; // cost of preference 

double gam = 2.0; // cost curvature of a preference (1: linear, 2: bell-shaped survival curve, 3: bell-shaped with skewness, etc etc)
double thet = 0.5; // how do preferences interact when it comes to a combined cost, see Pomiankowski & Iwasa 1993 procb
double c[ntrait_max]; // cost of trait
double lambda[ntrait_max]; // cost of trait

double biast[ntrait_max]; // mutation bias: 0.5 means no bias. > 0.5 means bias towards reduction in tratt.

double mu_p[ntrait_max];            // mutation rate preference
double mu_t[ntrait_max];            // mutation rate ornament
double sdmu_p[ntrait_max];			 // standard deviation mutation stepsize
double sdmu_t[ntrait_max];			 // standard deviation mutation stepsize
const double NumGen = 150000; // number of generations
const int skip = 10; // n generations interval before data is printed
double sexlimp = 0; // degree of sex-limited expression in p,t
double sexlimt = 0;
int pref = 0;
int web = 3;
double meanornsurv[ntrait_max];

int popsize = N; // population size between 
bool do_stats = 0;

int generation = 0;
int Nfemales = N / 2, Nmales = N / 2;
int msurvivors = 0;
int fsurvivors = 0;

int father_eggs[N];
int mother_eggs[N];


std::string file_name = "output.csv";

// the components of an actual individual
struct Individual
{
	double t[ntrait_max][2]; // diploid, additive loci for t,p
	double p[ntrait_max][2];
    	double t_expr[ntrait_max]; // and store their expressed values
    	double p_expr[ntrait_max];
	// amount to be allocated to male vs. female function
};

// generate the population
typedef Individual Population[N];
Population Females, Males, FemaleSurvivors, MaleSurvivors;
int Parents[N*clutch_size][2]; 

// function which obtains arguments from the command line
// for definitions of the various parameters see top of the file
void initArguments(int argc, char *argv[])
{
  
  for (int trait_idx = 0; trait_idx < ntrait; ++trait_idx)
  {
	a = std::stod(argv[1]);
	b = std::stod(argv[2]);
    // initially same cost for each ornament
	c[trait_idx] = std::stod(argv[3]);
  	lambda[trait_idx] = std::stod(argv[4]);
	biast[trait_idx] = std::stod(argv[5]);
	mu_p[trait_idx] = std::stod(argv[6]);
	mu_t[trait_idx] = std::stod(argv[7]);
	sdmu_p[trait_idx] = std::stod(argv[8]);
	sdmu_t[trait_idx] = std::stod(argv[9]);
	sexlimp = std::stod(argv[10]);
	sexlimt = std::stod(argv[11]);
    	pref = std::stoi(argv[12]);
    	init_t[trait_idx] = std::stod(argv[13]);
    	init_p[trait_idx] = std::stod(argv[14]);
    	gam = std::stod(argv[15]);
    	thet = std::stod(argv[16]);
     ntrait = std::stoi(argv[17]);
    	file_name = argv[18];
    //Maybe add another one to allow the change of ntraits to something other than 2? - NEED TO DO THIS
    //Get rid of fixed vector initalisation above, then use pushbacks here to keep them open-ended?
    //c = c.push_back(std::stod(argv[4]));
    //But then you'd need to change all the numbers of the args
    //Unless you just keep all initialisations for each variable the same across dimensions, then don't have to worry about this at all
    }
    
   for(int trait_idx = ntrait; trait_idx < ntrait_max; ++trait_idx)
     {
	      c[trait_idx] = 0.0;
  	    lambda[trait_idx] = 0.0;
        biast[trait_idx] = 0.5;
	      mu_p[trait_idx] = 0.0;
	      mu_t[trait_idx] = 0.0;
        sdmu_p[trait_idx] = 0.0;
	      sdmu_t[trait_idx] = 0.0;
  	    init_t[trait_idx] = 0.0;
    	  init_p[trait_idx] = 0.0;
     }
} // end initArguments

// mutation function:
// mutate an allele G given a particular mutation rate mu
// a standard deviation of the mutational effect distribution
// of sdmu and a certain mutational bias
void mutate(double &G, double mu, double sdmu, double bias=0.5)
{
    if (uniform(rng_r) < mu)
    {
        double effect = uniform_mutation(rng_r);
        G+= uniform(rng_r) < bias ? -effect : effect;
    }
}

// write the parameters to the DataFile
void WriteParameters(std::ofstream &DataFile)
{
	DataFile << std::endl
		<< std::endl
		<< "type:;" << "gonochorist_fisherian" << ";" << std::endl
		<< "popsize_init:;" << N << ";" << std::endl
		<< "n_mate_sample:;" << N_mate_sample << ";"<< std::endl
		<< "init_t:;" << init_t[0] << ";" << std::endl
		<< "init_p:;" << init_p[0] << ";" << std::endl
		<< "a:;" <<  a << ";"<< std::endl
		<< "b:;" <<  b << ";"<< std::endl
		<< "c:;" <<  c[0] << ";"<< std::endl
		<< "pref:;" <<  pref << ";"<< std::endl
		<< "mu_p:;" <<  mu_p[0] << ";" << std::endl
		<< "mu_t:;" <<  mu_t[0] << ";" << std::endl
		<< "mu_std_p:;" <<  sdmu_p[0] << ";" << std::endl
		<< "mu_std_t:;" <<  sdmu_t[0] << ";"<< std::endl
		<< "biast:;" <<  biast[0] << ";" << std::endl
		<< "sexlimp:;" <<  sexlimp << ";"<< std::endl
		<< "sexlimt:;" <<  sexlimt << ";"<< std::endl
    		<< "gamma:;" <<  gam << ";"<< std::endl
    		<< "theta:;" <<  thet << ";"<< std::endl
       <<"ntrait:;" << ntrait << ";" << std::endl
		<< "seed:;" << seed << ";"<< std::endl;
}

// initialize all the phenotypes
void Init()
{
	// initialize the whole population
	for (int i = 0; i < Nfemales; ++i)
	{
	   for (int trait_idx = 0; trait_idx < ntrait; ++trait_idx)
	   {
		// initialize both diploid loci
		for (int j = 0; j < 2; ++j)
		{
			Females[i].t[trait_idx][j] = init_t[trait_idx];
			Females[i].p[trait_idx][j] = init_p[trait_idx];
		}
		
		// and the expressed values
		Females[i].t_expr[trait_idx] = init_t[trait_idx];
		Females[i].p_expr[trait_idx] = init_p[trait_idx];
	  } // end for trait_idx
     
     //Set anything above ntrait up to ntrait_max to be 0
     for(int trait_idx = ntrait; trait_idx < ntrait_max; ++trait_idx)
     {
       		// initialize both diploid loci
		    for (int j = 0; j < 2; ++j)
		    {
			    Females[i].t[trait_idx][j] = 0.0;
			    Females[i].p[trait_idx][j] = 0.0;
	      	}
		
		    // and the expressed values
		    Females[i].t_expr[trait_idx] = 0.0;
		    Females[i].p_expr[trait_idx] = 0.0;
       
     }
     
	} // end for Nfemales

    // initialize the male part of the population
	for (int i = 0; i < Nmales; ++i)
	{
	   for (int trait_idx = 0; trait_idx < ntrait; ++trait_idx)
	   {
			for (int j = 0; j < 2; ++j)
			{
				Males[i].t[trait_idx][j] = init_t[trait_idx];
				Males[i].p[trait_idx][j] = init_p[trait_idx];
			}
				
		Males[i].t_expr[trait_idx] = init_t[trait_idx];
		Males[i].p_expr[trait_idx] = init_p[trait_idx];
	   }
    
    //Set anything above ntrait up to ntrait_max to be 0
     for(int trait_idx = ntrait; trait_idx < ntrait_max; ++trait_idx)
     {
       		// initialize both diploid loci
		    for (int j = 0; j < 2; ++j)
		    {
			    Males[i].t[trait_idx][j] = 0.0;
			    Males[i].p[trait_idx][j] = 0.0;
	      	}
		
		    // and the expressed values
		    Males[i].t_expr[trait_idx] = 0.0;
		    Males[i].p_expr[trait_idx] = 0.0;
       
     }
       
	}
} // end Init

// create an offspring 
void Create_Kid(int mother, int father, Individual &kid)
{
	assert(mother >= 0 && mother < fsurvivors);
	assert(father >= 0 && father < msurvivors);
 
 	for (int trait_idx = 0; trait_idx < ntrait; ++trait_idx)
 	{
  	   // inherit ornament
		kid.t[trait_idx][0] = FemaleSurvivors[mother].t[trait_idx][segregator(rng_r)];
   		mutate(kid.t[trait_idx][0], mu_t[trait_idx], sdmu_t[trait_idx], biast[trait_idx]);
		kid.t[trait_idx][1] = MaleSurvivors[father].t[trait_idx][segregator(rng_r)];
    		mutate(kid.t[trait_idx][1], mu_t[trait_idx], sdmu_t[trait_idx], biast[trait_idx]);

    	  // inherit preference
		kid.p[trait_idx][0] = FemaleSurvivors[mother].p[trait_idx][segregator(rng_r)];
   		mutate(kid.p[trait_idx][0], mu_p[trait_idx], sdmu_p[trait_idx]);
		kid.p[trait_idx][1] = MaleSurvivors[father].p[trait_idx][segregator(rng_r)];
    		mutate(kid.p[trait_idx][1], mu_p[trait_idx], sdmu_p[trait_idx]);
 	}
} // end Create_Kid

// survival stage
void Survive(std::ofstream &DataFile)
{
    // keep track of the 
    // number of female breeders
    fsurvivors = 0;     
    msurvivors = 0;
    
    // store individual fitness values
    double wf = 0; 
    double wm = 0; 

    // keep track of the 
    // mean ornament size of surviving males
    // necessary for absolute/relative preference
    // functions
    for (int trait_idx  = 0; trait_idx < ntrait; ++trait_idx)
    {
    meanornsurv[trait_idx] = 0.0;
    }
    
    double sump = 0.0;
    double sumctsq = 0.0;

    // allow females to survive
	for (int i = 0; i < Nfemales; ++i)
	{
	   sump = 0.0;

	   for (int trait_idx = 0; trait_idx < ntrait; ++trait_idx)
	   {
            	double p_expr = Females[i].p_expr[trait_idx];
   
            	sump += pow(lambda[trait_idx]*p_expr,1.0/thet);
	   }

	   // after summing over all traits, take power over gam * thet and multiply
	   // by b as in eq. (10b) in Pomiankowski & Iwasa (1993)
   	   wf = exp(-b*pow(sump,(gam * thet)));
      	   //std::cout << wf;

	   //w = exp(-b*p_expr*p_expr + (1-sexlimt)*(-c)*t_expr*t_expr);

	   // if individuals survive
	   // take stats and add them to pool of survivors
	   if (uniform(rng_r) < wf)
	    {
		    FemaleSurvivors[fsurvivors++] = Females[i];
	    }
	}// end for i < Nfemales


    // male survival
	for (int i = 0; i < Nmales; ++i)
	{
     	sumctsq = 0.0;
     
		for (int trait_idx = 0; trait_idx < ntrait; ++trait_idx)
    		{
			double t_expr = Males[i].t_expr[trait_idx];
        		sumctsq += c[trait_idx]*pow(t_expr,2);

     		} //end for trait_idx

      	//Eq. 10a in Pomiankowski & Iwasa (1993)
 	wm = exp(-sumctsq);      //wm(survival) = exp(-sumctsq);
        //std::cout << wm;
        
        	if (uniform(rng_r) < wm)
        	{
            		for (int trait_idx = 0; trait_idx < ntrait; ++trait_idx)
            		{
                		// in case of relative preferences get the mean ornament
                		meanornsurv[trait_idx] += Males[i].t_expr[trait_idx];
            		}

            	MaleSurvivors[msurvivors++] = Males[i];
       		}
	}

    // extinction?
    if (fsurvivors == 0 || msurvivors == 0)
    {
        WriteParameters(DataFile);
        exit(1);
    }

    for (int trait_idx  = 0; trait_idx < ntrait; ++trait_idx)
    {
        // take the average of the surviving male trait value
        meanornsurv[trait_idx] /= msurvivors;
    }

    assert(fsurvivors > 0);
    assert(fsurvivors < popsize);
    assert(msurvivors > 0);
    assert(msurvivors < popsize);
} // end void Survival()

double open_ended_prefs(Individual &female, Individual &male)
{
    double sum_odds = 0.0;

    for (int trait_idx  = 0; trait_idx < ntrait; ++trait_idx)
    {
        sum_odds += a * female.p_expr[trait_idx] * male.t_expr[trait_idx];
    }
    
    sum_odds = exp(sum_odds);

    //std::cout << sum_odds;
    return(sum_odds);
} // end open_ended_prefs()

double absolute_prefs(Individual &female, Individual &male)
{
    double sum_odds = 0.0;

    for (int trait_idx  = 0; trait_idx < ntrait; ++trait_idx)
    {
        	sum_odds += -0.5 * a * 
                (female.p_expr[trait_idx] - male.t_expr[trait_idx])*
                (female.p_expr[trait_idx] - male.t_expr[trait_idx]);
    }
    
    sum_odds = exp(sum_odds);

    return(sum_odds);
} // end absolute_prefs()

double relative_prefs(Individual &female, Individual &male)
{
    double sum_odds = 0.0;

    for (int trait_idx  = 0; trait_idx < ntrait; ++trait_idx)
    {
        	// check the eqn in Lande (1981) PNAS
        	sum_odds += -0.5 * a * 
                (male.t_expr[trait_idx] - (female.p_expr[trait_idx] + meanornsurv[trait_idx])) *
                (male.t_expr[trait_idx] - (female.p_expr[trait_idx] + meanornsurv[trait_idx]));
    }
    
    sum_odds = exp(sum_odds);

    return(sum_odds);
} // end relative_prefs()

double weber_prefs(Individual &female, Individual &male)
{
    double sum_odds = 0.0;
    
    for (int trait_idx  = 0; trait_idx < ntrait; ++trait_idx)
    {
        sum_odds += a * (male.t_expr[trait_idx] / (male.t_expr[trait_idx] + female.p_expr[trait_idx]));
    }

    return(sum_odds);
} // end weber_prefs()


// mate choice - Kathryn suggested new function
void Choose(Individual &mother, int &father) 
{
    // mate choice - original code
	// make arrays that hold the values of the sample of assessed males
	double Fitness[N_mate_sample];
	int Candidates[N_mate_sample];

	double sumFitness = 0;

	// check if there are enough other males to choose from
	// otherwise restrict the sample to the amount of individuals present
	int current_mate_sample = N_mate_sample > msurvivors ? 
        msurvivors 
        : 
        N_mate_sample;

    // distribution of number of survivors
    	std::uniform_int_distribution <int> 
        msurvivor_sampler(0, msurvivors - 1);

    // mate choice among the sample of males
	for (int j = 0; j < current_mate_sample; ++j)
	{
        	// get a random surviving male
        	int random_mate = msurvivor_sampler(rng_r);

        	// value of the preference function
        	double po = 0;

        	switch(pref)
        	{
           		 // open-ended preferences
            		case 0: 
            		{
               		 	po = open_ended_prefs(mother, MaleSurvivors[random_mate]);
                		//std::cout << po;
            		} break;

            		// absolute preferences
            		case 1:
            		{
                		po = absolute_prefs(mother, MaleSurvivors[random_mate]);
            		} break;

            		// relative preferences
            		case 2:
            		{
                		po = relative_prefs(mother, MaleSurvivors[random_mate]);
            		} break;
            
            		// Weber preferences
            		case 3:
            		{
                		po = weber_prefs(mother, MaleSurvivors[random_mate]);
            		} break;
        	} // end switch

        	// prevent the exponential of going to infinity
        	// which would make things really awkward
        	if (po > 100)
        	{
            		po = 100;
        	}

        	// make a cumulative distribution of the male's
        	// fitness value
        	Fitness[j] = sumFitness + po;
        	sumFitness=Fitness[j];

        	Candidates[j] = random_mate;
    	} 

    // sample from the cumulative distribution
	double r = uniform(rng_r)*sumFitness;

    // by default mate randomly
    // e.g., if the cumulative distribution is flat
	father = msurvivor_sampler(rng_r);

    // probability that a male is chosen is proportional
    // to the size of his share in the cumulative distribution
	for (int j = 0; j < current_mate_sample; ++j)
	{
		if (r <= Fitness[j])
		{
			father=Candidates[j];				
			break;	
		}
	}
 
    assert(father >= 0 && father < msurvivors);

} // end ChooseMates


// produce the next generation
void NextGen()
{
    int offspring = 0;

    // do some stats of the adult reproductive
    // population
    if (do_stats)
    {
        for (int i = 0; i < msurvivors; ++i)
        {
            father_eggs[i] = 0;
        }

        for (int i = 0; i < fsurvivors; ++i)
        {
            mother_eggs[i] = 0;
        }
    }

    // let the surviving females choose a mate
	for (int i = 0; i < fsurvivors; ++i)
	{
            int Father = -1;
            
            Choose(FemaleSurvivors[i], Father);

            assert(Father >= 0 && Father < msurvivors);

            // do some stats on the mating pairs
            if (do_stats)
            {
                mother_eggs[i] += clutch_size;
                father_eggs[Father] += clutch_size;
            }

            // for each offspring to be produced
            // store the indices of the parents
            // we then make offspring later
            for (int j = 0; j < clutch_size; ++j)
            {
                Parents[offspring][0] = i;
                Parents[offspring][1] = Father;
                ++offspring;
            }    
 	}

    assert(offspring <= N*clutch_size);
    assert(offspring >= 0);
    
    int sons = 0;
    int daughters = 0;

    // the size of the resulting population in the next 
    // generation is dependent on the number of 
    // offspring produced
    popsize = offspring < N ? offspring : N;

    std::uniform_int_distribution <int> offspring_sampler(0, offspring - 1);

    // replace the next generation
    for (int i = 0; i < popsize; ++i)
    {
        // create an offspring
        Individual Kid;

        int random_offspring = offspring_sampler(rng_r);

        // randomly sample an offspring to replace the population
        Create_Kid(Parents[random_offspring][0], Parents[random_offspring][1], Kid);

        assert(Parents[random_offspring][0] >= 0); 
        assert(Parents[random_offspring][0] < fsurvivors);
        assert(Parents[random_offspring][1] >= 0);
        assert(Parents[random_offspring][1] < msurvivors);

        // it's a boy
        if (uniform(rng_r) < 0.5)
        {
            Males[sons] = Kid;

            
            for (int trait_idx = 0; trait_idx < ntrait; ++trait_idx)
            {
                double t = 0.5 * ( Males[sons].t[trait_idx][0] + Males[sons].t[trait_idx][1]);
                double p = 0.5 * ( Males[sons].p[trait_idx][0] + Males[sons].p[trait_idx][1]);

                Males[sons].t_expr[trait_idx] = t;
                Males[sons].p_expr[trait_idx] = p;
            }
            ++sons;
        }
        else
        {
            Females[daughters] = Kid;

            for (int trait_idx = 0; trait_idx < ntrait; ++trait_idx)
            {
                double t = 0.5 * ( Females[daughters].t[trait_idx][0] + Females[daughters].t[trait_idx][1]);
                double p = 0.5 * ( Females[daughters].p[trait_idx][0] + Females[daughters].p[trait_idx][1]);
                Females[daughters].t_expr[trait_idx] = t;
                Females[daughters].p_expr[trait_idx] = p;
            }
            ++daughters;
        }
    }

    Nmales = sons;
    Nfemales = daughters;

    assert(Nmales <= N);
    assert(Nfemales <= N);
    assert(Nmales <= N);
    assert(Nfemales <= N);
} // end NextGen


// write the data
void WriteData(std::ofstream &DataFile)
{
    // in case population is extinct,
    // quit
	if (Nmales == 0 || Nfemales == 0)
	{
		WriteParameters(DataFile);
		exit(1);
	}

    double meanp[ntrait_max];
    double meant[ntrait_max]; 
    double ssp[ntrait_max];
    double sst[ntrait_max];
    double spt[ntrait_max];

    double p[ntrait_max],t[ntrait_max],meanmrs,meanfrs,varfrs,varmrs;
    double ssmrs = 0, ssfrs = 0, summrs=0, sumfrs=0;

    // calculate means and variances for the males
	for (int i = 0; i < Nmales; ++i)
	{
   		for (int trait_idx = 0; trait_idx < ntrait_max; ++trait_idx)
   		{
			p[trait_idx] = 0.5 * ( Males[i].p[trait_idx][0] + Males[i].p[trait_idx][1]);
			t[trait_idx] = 0.5 * ( Males[i].t[trait_idx][0] + Males[i].t[trait_idx][1]);

        		meanp[trait_idx] += p[trait_idx];
        		meant[trait_idx] += t[trait_idx];

        		ssp[trait_idx] += p[trait_idx] * p[trait_idx];
        		sst[trait_idx] += t[trait_idx] * t[trait_idx];
        		spt[trait_idx] += t[trait_idx] * p[trait_idx];
    		}

        	if (i < msurvivors)
        	{
            		summrs += father_eggs[i];
            		ssmrs += father_eggs[i] * father_eggs[i];
        	}
	}

    // calculate means and variances for the females
	for (int i = 0; i < Nfemales; ++i)
	{
   		for (int trait_idx = 0; trait_idx < ntrait_max; ++trait_idx)
   		{
			p[trait_idx] = 0.5 * ( Females[i].p[trait_idx][0] + Females[i].p[trait_idx][1]);
			t[trait_idx] = 0.5 * ( Females[i].t[trait_idx][0] + Females[i].t[trait_idx][1]);

        		meanp[trait_idx] += p[trait_idx];
        		meant[trait_idx] += t[trait_idx];

        		ssp[trait_idx] += p[trait_idx] * p[trait_idx];
        		sst[trait_idx] += t[trait_idx] * t[trait_idx];
        		spt[trait_idx] += t[trait_idx] * p[trait_idx];
    		}
        
        	if (i < fsurvivors)
        	{
            		sumfrs += mother_eggs[i];
            		ssfrs += mother_eggs[i] * mother_eggs[i];
        	}
	} 

    double varp[ntrait_max]; 
    double vart[ntrait_max]; 
    double covpt[ntrait_max];

    double sum_sexes = Nmales + Nfemales;

    for (int trait_idx = 0; trait_idx < ntrait_max; ++trait_idx)
    {
        meant[trait_idx] /= sum_sexes;
        meanp[trait_idx] /= sum_sexes;
        vart[trait_idx] = sst[trait_idx] / sum_sexes - meant[trait_idx] * meant[trait_idx];
        varp[trait_idx] = ssp[trait_idx] / sum_sexes - meanp[trait_idx] * meanp[trait_idx];
        covpt[trait_idx] = spt[trait_idx] / sum_sexes - meanp[trait_idx] * meant[trait_idx];
        
    }

    

    meanfrs = sumfrs / Nfemales;
    varfrs = ssfrs / Nfemales - meanfrs * meanfrs;

    meanmrs = summrs / Nmales;
    varmrs = ssmrs / Nmales - meanmrs * meanmrs;


    //To check that p2 and t2 stay at 0
    //assert(meanp[1] == 0.0 && meant[1] == 0.0);

    // output of all the statistics
	DataFile << generation << ";";
    	for (int trait_idx = 0; trait_idx < ntrait_max; ++trait_idx)
    	{
		DataFile << meanp[trait_idx]
                << ";" << meant[trait_idx]
                << ";" << varp[trait_idx]
                << ";" << vart[trait_idx]
                << ";" << covpt [trait_idx]
                << ";";
    	}
	DataFile << meanfrs 
        << ";" << meanmrs 		
        << ";" << varfrs 
        << ";" << varmrs 
        << ";" << msurvivors
        << ";" << fsurvivors
	<< ";" << sum_sexes
	<< ";" << std::endl;
         
} // end WriteData

// headers of the datafile
void WriteDataHeaders(std::ofstream &DataFile)
{
    DataFile << "generation" << ";";

    for (int trait_idx = 0; trait_idx < ntrait_max; ++trait_idx)
    {
	    DataFile 
            << "meanp" << (trait_idx + 1) << ";"
            << "meant" << (trait_idx + 1) << ";"
            << "varp" << (trait_idx + 1) << ";"
            << "vart" << (trait_idx + 1) << ";"
            << "covpt" << (trait_idx + 1) << ";";
    }

    DataFile << "meanfrs"
        << ";meanmrs"
        << ";varfrs"
        << ";varmrs"
        << ";surviving_males"
        << ";surviving_females"
	<< ";N;"
	<< std::endl;
} // end WriteDataHeaders

// the core part of the code
int main(int argc, char ** argv)
{
	initArguments(argc, argv);

    // initialize output file
    std::ofstream output_file(file_name.c_str());

	WriteDataHeaders(output_file);
	Init();

	for (generation = 0; generation <= NumGen; ++generation)
	{
		do_stats = generation % skip == 0;

		Survive(output_file);
        
        NextGen();
        
        if (do_stats)
		{
			WriteData(output_file);
		}
	}

	WriteParameters(output_file);
} // end main
