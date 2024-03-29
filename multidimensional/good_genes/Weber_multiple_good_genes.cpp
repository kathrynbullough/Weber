//      Good genes sexual selection for gonochorists with addition of a Weber preference function

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
#include <algorithm>

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

int const ntrait = 2;

double init_t[ntrait] = {0.0,0.0}; // initial value for ornament
double init_p[ntrait] = {0.0,0.0}; // initial value for preference
double init_v = 0.0; // initial value for viability
double a = 1.0; // choice slope
double b = 0.5; // cost of preference 

double gam = 2.0; // cost curvature of a preference (1: linear, 2: bell-shaped survival curve, 3: bell-shaped with skewness, etc etc)
                  //
double thet = 0.5; // how do preferences interact when it comes to a combined cost, see Pomiankowski & Iwasa 1993 procb
double c[ntrait] = {0.5,0.5}; // cost of trait
double lambda[ntrait] = {1.0,1.0}; // cost of trait

double k[ntrait] = {0.0,0.0}; // sensitivity of cost coefficient to general viability, see 2nd equation of eq. (13) Iwasa & Pomiankowski 1994 

double biast[ntrait] = {0.0,0.0}; // mutation bias: 0.5 means no bias. > 0.5 means bias towards reduction in tratt.

double biasv{0.0}; // mutation bias acting on the general viability trait
                                  //
double mu_p[ntrait] 	  = {0.05,0.05};            // mutation rate preference
double mu_t[ntrait] 	  = {0.05,0.05};            // mutation rate ornament
double mu_v 	  = 0.05;            //mutation rate viability
double sdmu_p[ntrait]         = {0.4,0.4};			 // standard deviation mutation stepsize
double sdmu_t[ntrait]         = {0.4,0.4};			 // standard deviation mutation stepsize
double sdmu_v         = 0.4;					 // neg binomial mutational effect size parameter (w in Iwasa et al (1991) and w in Fawcett et al 2007 p. 75
double v_opt = 2.0;

const double NumGen = 150000; // number of generations
const int skip = 10; // n generations interval before data is printed
double sexlimp = 0; // degree of sex-limited expression in p,t
double sexlimt = 0;
int pref = 0;
int web = 3;
double meanornsurv[2] = {0.0,0.0};

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
    double t[ntrait][2]; // diploid, additive loci for t,p
    double p[ntrait][2];
    double v[2]; // extra locus for v
    double t_expr[ntrait]; // and store their expressed values
    double p_expr[ntrait];
    double v_expr;
	// amount to be allocated to male vs. female function
};

// generate the population
typedef Individual Population[N];
Population Females, Males, FemaleSurvivors, MaleSurvivors;
int Parents[N*clutch_size][2]; //does this need to be 3?

// function which obtains arguments from the command line
// for definitions of the various parameters see top of the file
void initArguments(int argc, char *argv[])
{
	a = std::stod(argv[1]);
	b = std::stod(argv[2]);
	c[0] = std::stod(argv[3]);
  	c[1] = std::stod(argv[4]);
  	lambda[0] = lambda[1] = std::stod(argv[5]);
	biast[0] = std::stod(argv[6]);
  	biast[1] = std::stod(argv[7]);

    // bias in the genetic quality trait
  	biasv = std::stod(argv[8]);

	mu_p[0] = std::stod(argv[9]);
  	mu_p[1] = std::stod(argv[10]);
	mu_t[0] = std::stod(argv[11]);
 	mu_t[1] = std::stod(argv[12]);
	mu_v = std::stod(argv[13]);
	sdmu_p[0] = std::stod(argv[14]);
  	sdmu_p[1] = std::stod(argv[15]);
	sdmu_t[0] = std::stod(argv[16]);
  	sdmu_t[1] = std::stod(argv[17]);    
    sdmu_v = std::stod(argv[18]);
	sexlimp = std::stod(argv[19]);
	sexlimt = std::stod(argv[20]);
    	pref = std::stoi(argv[21]);
    	init_t[0] = std::stod(argv[22]);
    	init_t[1] = std::stod(argv[23]);
    	init_p[0] = std::stod(argv[24]);
    	init_p[1] = std::stod(argv[25]);
      init_v = std::stod(argv[26]);
    	gam = std::stod(argv[27]);
    	thet = std::stod(argv[28]);
     k[0] = std::stod(argv[29]);
     k[1] = std::stod(argv[30]);
     v_opt = std::stod(argv[31]);
    	file_name = argv[32];
    //Maybe add another one to allow the change of ntraits to something other than 2?
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
} // end mutate

// write the parameters to the DataFile
void WriteParameters(std::ofstream &DataFile)
{
	DataFile << std::endl
		<< std::endl
		<< "type:;" << "gonochorist_fisherian" << ";" << std::endl
		<< "popsize_init:;" << N << ";" << std::endl
		<< "n_mate_sample:;" << N_mate_sample << ";"<< std::endl
		<< "init_t1:;" << init_t[0] << ";" << std::endl
    		<< "init_t2:;" << init_t[1] << ";" << std::endl
		<< "init_p1:;" << init_p[0] << ";" << std::endl
    		<< "init_p2:;" << init_p[1] << ";" << std::endl
       << "init_v:;" << init_v << ";" << std::endl
		<< "a:;" <<  a << ";"<< std::endl
		<< "b:;" <<  b << ";"<< std::endl
		<< "c1:;" <<  c[0] << ";"<< std::endl
   		<< "c2:;" <<  c[1] << ";"<< std::endl
		<< "pref:;" <<  pref << ";"<< std::endl
		<< "mu_p1:;" <<  mu_p[0] << ";" << std::endl
    		<< "mu_p2:;" <<  mu_p[1] << ";" << std::endl
            << "mu_t1:;" <<  mu_t[0] << ";" << std::endl
    		<< "mu_t2:;" <<  mu_t[1] << ";" << std::endl
            << "mu_v:;" <<  mu_v << ";" << std::endl
		<< "mu_std_p1:;" <<  sdmu_p[0] << ";" << std::endl
    		<< "mu_std_p2:;" <<  sdmu_p[1] << ";" << std::endl
		<< "mu_std_t1:;" <<  sdmu_t[0] << ";"<< std::endl
    		<< "mu_std_t2:;" <<  sdmu_t[1] << ";" << std::endl
		<< "biast1:;" <<  biast[0] << ";" << std::endl
    		<< "biast2:;" <<  biast[1] << ";" << std::endl
    		<< "biasv:;" <<  biasv << ";" << std::endl
    		<< "v_opt:;" <<  v_opt << ";" << std::endl
		<< "sexlimp:;" <<  sexlimp << ";"<< std::endl
		<< "sexlimt:;" <<  sexlimt << ";"<< std::endl
    		<< "gamma:;" <<  gam << ";"<< std::endl
    		<< "theta:;" <<  thet << ";"<< std::endl
       << "mu_v:;" << mu_v << ";" << std::endl
       << "sdmu_v:;" << sdmu_v << ";" << std::endl
       << "k1:;" << k[0] << ";" << std::endl
       << "k2:;" << k[1] << ";" << std::endl
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
			//Females[i].t[trait_idx][j] = init_t[trait_idx];
			//Females[i].p[trait_idx][j] = init_p[trait_idx];
            Females[i].t[trait_idx][j] = init_t[trait_idx];
			Females[i].p[trait_idx][j] = init_p[trait_idx];
		}
		
		// and the expressed values
		//Females[i].t_expr[trait_idx] = init_t[trait_idx];
		//Females[i].p_expr[trait_idx] = init_p[trait_idx];
         Females[i].t_expr[trait_idx] = init_t[trait_idx]*init_v;
         Females[i].p_expr[trait_idx] = init_p[trait_idx];
        } // end for trait_idx
     
         //initialise v
         for (int j = 0; j < 2; ++j)
         {
             Females[i].v[j] = init_v;
         }
         Females[i].v_expr = init_v;
     
	} // end for Nfemales

    // initialize the male part of the population
	for (int i = 0; i < Nmales; ++i)
	{
	   for (int trait_idx = 0; trait_idx < ntrait; ++trait_idx)
	   {
			for (int j = 0; j < 2; ++j)
			{
				//Males[i].t[trait_idx][j] = init_t[trait_idx];
				//Males[i].p[trait_idx][j] = init_p[trait_idx];
                Males[i].t[trait_idx][j] = init_t[trait_idx];
				Males[i].p[trait_idx][j] = init_p[trait_idx];
			}
				
		//Males[i].t_expr[trait_idx] = init_t[trait_idx];
		//Males[i].p_expr[trait_idx] = init_p[trait_idx];
            Males[i].t_expr[trait_idx] = init_t[trait_idx]*init_v;
            Males[i].p_expr[trait_idx] = init_p[trait_idx];
	   }
       
        //initialise v
         for (int j = 0; j < 2; ++j)
         {
             Males[i].v[j] = init_v;
         }
         Males[i].v_expr = init_v;  
    
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
  
     // inherit viability
    kid.v[0] = FemaleSurvivors[mother].v[segregator(rng_r)];
    mutate(kid.v[0], mu_v, sdmu_v, biasv);
    kid.v[1] = MaleSurvivors[father].v[segregator(rng_r)];
    mutate(kid.v[1], mu_v, sdmu_v, biasv);


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
    meanornsurv[0] = 0.0;
    meanornsurv[1] = 0.0;
    
    double sump = 0.0;
    double sumdiv = 0.0;

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
	   // by b, then add to v, as in eq. (13) in  Iwasa & Pomiankowski (1994)
       double v_expr = Females[i].v_expr;

       // exp(-v_expr^2) is stabilizing selection around
   	   wf = exp(-b*pow(sump,(gam * thet)) -(v_expr - v_opt)*(v_expr - v_opt));
          //Or is v_expr meant to be out of the exp??
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
        sumdiv = 0.0;
        double v_expr = Males[i].v_expr;
     
		for (int trait_idx = 0; trait_idx < ntrait; ++trait_idx)
        {
            double t_expr = Males[i].t_expr[trait_idx];
            sumdiv += (c[trait_idx]/(1+k[trait_idx]*v_expr))*pow(t_expr,2);

        } //end for trait_idx

      	//Eq. 2A, 13 in Iwasa & Pomiankowski (1994)
        wm = exp(-sumdiv -(v_expr - v_opt)*(v_expr - v_opt));      //wm(survival) = exp(-sumctsq);
  //Or is v_expr meant to be out of the exp??
        //std::cout << wm;
        
        if (uniform(rng_r) < wm)
        {
            for (int trait_idx = 0; trait_idx < ntrait; ++trait_idx)
            {
                // in case of relative preferences get the mean ornament
           //Does this need to be multiplied by v as well??
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


// mate choice
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

            double v = 0.5 * ( Males[sons].v[0] + Males[sons].v[1]);
            Males[sons].v_expr = v;  
            
            for (int trait_idx = 0; trait_idx < ntrait; ++trait_idx)
            {
                double t = 0.5 * ( Males[sons].t[trait_idx][0] + Males[sons].t[trait_idx][1]);
                double p = 0.5 * ( Males[sons].p[trait_idx][0] + Males[sons].p[trait_idx][1]);

                Males[sons].t_expr[trait_idx] = t*v;
                Males[sons].p_expr[trait_idx] = p;
            }
            ++sons;
        }
        else
        {
            Females[daughters] = Kid;
            
            double v = 0.5 * ( Females[daughters].v[0] + Females[daughters].v[1]);
            Females[daughters].v_expr = v;

            for (int trait_idx = 0; trait_idx < ntrait; ++trait_idx)
            {
                double t = 0.5 * ( Females[daughters].t[trait_idx][0] + Females[daughters].t[trait_idx][1]);
                double p = 0.5 * ( Females[daughters].p[trait_idx][0] + Females[daughters].p[trait_idx][1]);
                Females[daughters].t_expr[trait_idx] = t*v;
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

    double meanp[ntrait] = {0.0,0.0};
    double meant[ntrait] = {0.0,0.0};
    double meantexpr[ntrait] = {0.0,0.0};
    double ssp[ntrait] = {0.0,0.0};
    double sst[ntrait] = {0.0,0.0};
    double sstexpr[ntrait] = {0.0,0.0};
    double spt[ntrait] = {0.0,0.0};
    double sptexpr[ntrait] = {0.0,0.0};
    double spv[ntrait] = {0.0,0.0};
    double stv[ntrait] = {0.0,0.0};
    double meanv = 0.0;
    double ssv = 0.0;

    double p[ntrait],t[ntrait],texpr[ntrait],v,meanmrs,meanfrs,varfrs,varmrs;
    double ssmrs = 0, ssfrs = 0, summrs=0, sumfrs=0;

    // calculate means and variances for the males
	for (int i = 0; i < Nmales; ++i)
	{
   		for (int trait_idx = 0; trait_idx < ntrait; ++trait_idx)
   		{
			p[trait_idx] = 0.5 * ( Males[i].p[trait_idx][0] + Males[i].p[trait_idx][1]);
			t[trait_idx] = 0.5 * ( Males[i].t[trait_idx][0] + Males[i].t[trait_idx][1]);
      texpr[trait_idx] = Males[i].t_expr[trait_idx];

        		meanp[trait_idx] += p[trait_idx];
        		meant[trait_idx] += t[trait_idx];
            meantexpr[trait_idx] += texpr[trait_idx];
        		ssp[trait_idx] += p[trait_idx] * p[trait_idx];
        		sst[trait_idx] += t[trait_idx] * t[trait_idx];
            sstexpr[trait_idx] += texpr[trait_idx] * texpr[trait_idx];
        		spt[trait_idx] += t[trait_idx] * p[trait_idx];
            sptexpr[trait_idx] += texpr[trait_idx] * p[trait_idx];
    		}
       
       v = 0.5 * (Males[i].v[0] + Males[i].v[1]);
       meanv += v;
       ssv += v*v;
       
       for (int trait_idx = 0; trait_idx < ntrait; ++trait_idx)
   		{
        spv[trait_idx] += p[trait_idx] * v;
        stv[trait_idx] += t[trait_idx] * v;
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
   		for (int trait_idx = 0; trait_idx < ntrait; ++trait_idx)
   		{
			p[trait_idx] = 0.5 * ( Females[i].p[trait_idx][0] + Females[i].p[trait_idx][1]);
			t[trait_idx] = 0.5 * ( Females[i].t[trait_idx][0] + Females[i].t[trait_idx][1]);
      texpr[trait_idx] = Females[i].t_expr[trait_idx];

        		meanp[trait_idx] += p[trait_idx];
        		meant[trait_idx] += t[trait_idx];
            meantexpr[trait_idx] += texpr[trait_idx];

        		ssp[trait_idx] += p[trait_idx] * p[trait_idx];
        		sst[trait_idx] += t[trait_idx] * t[trait_idx];
            sstexpr[trait_idx] += texpr[trait_idx] * texpr[trait_idx];
        		spt[trait_idx] += t[trait_idx] * p[trait_idx];
            sptexpr[trait_idx] += texpr[trait_idx] * p[trait_idx];
    		}
       
       v = 0.5 * (Females[i].v[0] + Females[i].v[1]);
       meanv += v;
       ssv += v*v;
       
       for (int trait_idx = 0; trait_idx < ntrait; ++trait_idx)
   		{
        spv[trait_idx] += p[trait_idx] * v;
        stv[trait_idx] += t[trait_idx] * v;
       }
        
        	if (i < fsurvivors)
        	{
            		sumfrs += mother_eggs[i];
            		ssfrs += mother_eggs[i] * mother_eggs[i];
        	}
	} 

    double varp[ntrait] = {0.0,0.0}; 
    double vart[ntrait] = {0.0,0.0}; 
    double vartexpr[ntrait] = {0.0,0.0};
    double covpt[ntrait] = {0.0,0.0};
    double covptexpr[ntrait] = {0.0,0.0};
    double covpv[ntrait] = {0.0,0.0};
    double covtv[ntrait] = {0.0,0.0};
    double varv = 0.0;

    double sum_sexes = Nmales + Nfemales;

    for (int trait_idx = 0; trait_idx < ntrait; ++trait_idx)
    {
        meant[trait_idx] /= sum_sexes;
        meantexpr[trait_idx] /= sum_sexes;
        meanp[trait_idx] /= sum_sexes;
        vart[trait_idx] = sst[trait_idx] / sum_sexes - meant[trait_idx] * meant[trait_idx];
        vartexpr[trait_idx] = sstexpr[trait_idx] / sum_sexes - meantexpr[trait_idx] * meantexpr[trait_idx];
        varp[trait_idx] = ssp[trait_idx] / sum_sexes - meanp[trait_idx] * meanp[trait_idx];
        covpt[trait_idx] = spt[trait_idx] / sum_sexes - meanp[trait_idx] * meant[trait_idx];
        covptexpr[trait_idx] = sptexpr[trait_idx] / sum_sexes - meanp[trait_idx] * meantexpr[trait_idx];
        
    }
    
    meanv /= sum_sexes;
    varv = ssv / sum_sexes - meanv * meanv;
    
    for (int trait_idx = 0; trait_idx < ntrait; ++trait_idx)
    {
      covpv[trait_idx] = spv[trait_idx] / sum_sexes - meanp[trait_idx] * meanv;
      covtv[trait_idx] = stv[trait_idx] / sum_sexes - meant[trait_idx] * meanv;
    }

    meanfrs = sumfrs / Nfemales;
    varfrs = ssfrs / Nfemales - meanfrs * meanfrs;

    meanmrs = summrs / Nmales;
    varmrs = ssmrs / Nmales - meanmrs * meanmrs;


    //To check that p2 and t2 stay at 0
    //assert(meanp[1] == 0.0 && meant[1] == 0.0);

    // output of all the statistics
	DataFile << generation << ";";
    	for (int trait_idx = 0; trait_idx < ntrait; ++trait_idx)
    	{
		DataFile << meanp[trait_idx]
                << ";" << meant[trait_idx]
                << ";" << meantexpr[trait_idx]
                << ";" << varp[trait_idx]
                << ";" << vart[trait_idx]
                << ";" << vartexpr[trait_idx]
                << ";" << covpt [trait_idx]
                << ";" << covptexpr [trait_idx]
                << ";" << covpv [trait_idx]
                << ";" << covtv [trait_idx]
                << ";";
    	}
	DataFile << meanv
 << ";" << varv
  << ";" << meanfrs 
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

    for (int trait_idx = 0; trait_idx < ntrait; ++trait_idx)
    {
	    DataFile 
            << "meanp" << (trait_idx + 1) << ";"
            << "meant" << (trait_idx + 1) << ";"
            << "meantexpr" << (trait_idx + 1) << ";"
            << "varp" << (trait_idx + 1) << ";"
            << "vart" << (trait_idx + 1) << ";"
            << "vartexpr" << (trait_idx + 1) << ";"
            << "covpt" << (trait_idx + 1) << ";"
            << "covptexpr" << (trait_idx + 1) << ";"
            << "covpv" << (trait_idx + 1) << ";"
            << "covtv" << (trait_idx + 1) << ";";
    }

    DataFile << "meanv"
    << ";varv"
    << ";meanfrs"
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
