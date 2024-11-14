#include <string>
#include "good_genes.hpp"
#include "parameters.hpp"

int main(int argc, char **argv)
{
    Parameters parameters;

    parameters.biasv = std::stod(argv[1]);
    parameters.a = std::stod(argv[2]);
    parameters.b = std::stod(argv[3]);
    parameters.c = std::stod(argv[4]);
    parameters.gam = std::stod(argv[5]);
    parameters.thet = std::stod(argv[6]);
    parameters.init_t = std::stod(argv[7]);
    parameters.init_p = std::stod(argv[8]);
    parameters.max_num_gen = std::stoi(argv[9]);
    parameters.pref = std::stoi(argv[10]);
    parameters.ntrait = std::stoi(argv[11]);
    parameters.file_name = argv[12];
    GoodGenes gg(parameters);

} // end main
