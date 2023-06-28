# executable name
EXE=Weber_multiple.exe

# c++ files needed to build
CPP=Weber_multiple.cpp

# which compiler?
CXX=g++

# any flags that the compiler uses?
CXXFLAGS=-Wall -O3 -std=c++17

# actually building the executable, this will run
# if you type 'make'
$(EXE) : $(CPP)
	$(CXX) $(CXXFLAGS) -o $(EXE) $(CPP)

# type 'make clean' in case you want to remove the exe
clean : 
	rm -rf $(EXE)
