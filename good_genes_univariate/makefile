EXE=good_genes.exe
CPP=good_genes.cpp individual.cpp main.cpp
HPP=good_genes.hpp individual.hpp parameters.hpp
CXX=g++
CXXFLAGS=-Wall -O3 -std=c++20

$(EXE) : $(CPP) $(HPP)
	$(CXX) $(CXXFLAGS) -o $(EXE) $(CPP) 


clean :
	rm -rf $(EXE)
