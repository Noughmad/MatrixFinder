#ifndef MATRIXFINDER_H
#define MATRIXFINDER_H

#include <string>
#include <list>

#include <CL/cl.hpp>

class Matrix
{
public:
	Matrix(const std::string& file);
	Matrix(int rows, int cols);
	~Matrix();

	int nRows;
	int nCols;
	char* data;
};

struct Coordinates
{
	int row;
	int column;
};

class MatrixFinder
{
public:
	MatrixFinder(const Matrix& data, const Matrix& pattern, const std::string& openclFile);
	
	void setupOpenCL();
	std::vector<Coordinates> find();
	
private:
	Matrix mData;
	Matrix mPattern;
	std::string mOpenclFile;
	
	cl::Context mContext;
	cl::Program mProgram;
	cl::Kernel mFindKernel;
	cl::Device mDevice;
};

#endif // MATRIXFINDER_H
