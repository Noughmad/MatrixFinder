#include <iostream>

#include "matrixfinder.h"

int main(int argc, char **argv) {   
    Matrix data(argv[1]);
    Matrix pattern(argv[2]);
        
    MatrixFinder finder(data, pattern, argv[3]);
    finder.setupOpenCL();
    
    std::vector<Coordinates> coordinates = finder.find();
    for (Coordinates c : coordinates)
    {
	std::cout << '(' << c.row << "," << c.column << ')' << std::endl;
    }
    
    return 0;
}
