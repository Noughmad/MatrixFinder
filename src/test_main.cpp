#include <iostream>

#include "matrixfinder.h"

int main(int argc, char **argv) {   
    Matrix data(argv[1]);
    Matrix pattern(argv[2]);
    
    MatrixFinder finder(data, pattern, argv[3]);
    finder.setupOpenCL();
    
    std::vector<Coordinates> coordinates = finder.find();
    
    if (coordinates.size() != 2)
    {
	std::cerr << "Result size is " << coordinates.size() << " instead of 2" << std::endl;
	return 1;
    }
    
    if (coordinates[0].row != 1 || coordinates[0].column != 4)
    {
   	std::cerr << "Result item 0 is (" << coordinates[0].row << "," << coordinates[0].column << ") instead of (1,4)" << std::endl;
	return 2;
    }
    if (coordinates[1].row != 2 || coordinates[1].column != 1)
    {
   	std::cerr << "Result item 1 is (" << coordinates[1].row << "," << coordinates[1].column << ") instead of (2,1)" << std::endl;
	return 3;
    }
    
    std::cerr << "Done" << std::endl;
    return 0;
}
