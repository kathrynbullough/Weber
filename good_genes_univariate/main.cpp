#include <string>
#include "good_genes.hpp"
#include "parameters.hpp"

int main(int argc, char **argv)
{
    Parameters parameters;

    parameters.biasv = std::stod(argv[1]);
    parameters.max_num_gen = std::stoi(argv[2]);
    parameters.file_name = argv[3];
    GoodGenes gg(parameters);

} // end main
