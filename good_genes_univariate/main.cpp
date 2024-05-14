#include <string>
#include "good_genes.hpp"
#include "parameters.hpp"

int main(int argc, char **argv)
{
    Parameters parameters;

    parameters.biasv = std::stod(argv[1]);
    parameters.max_num_gen = std::stoi(argv[2]);
    parameters.pref = std::stoi(argv[3]);
    parameters.file_name = argv[4];
    GoodGenes gg(parameters);

} // end main
