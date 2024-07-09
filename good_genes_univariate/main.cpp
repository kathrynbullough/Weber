#include <string>
#include "good_genes.hpp"
#include "parameters.hpp"

int main(int argc, char **argv)
{
    Parameters parameters;

    parameters.biasv = std::stod(argv[1]);
    parameters.c = std::stod(argv[2]);
    parameters.max_num_gen = std::stoi(argv[3]);
    parameters.pref = std::stoi(argv[4]);
    parameters.ntrait = std::stoi(argv[5]);
    parameters.file_name = argv[6];
    GoodGenes gg(parameters);

} // end main
