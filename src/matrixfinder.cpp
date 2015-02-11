#include "matrixfinder.h"

#include <fstream>
#include <vector>
#include <sstream>
#include <iostream>

#include <CL/cl.hpp>

Matrix::Matrix(int rows, int cols)
: nRows(rows)
, nCols(cols)
{
	data = new char[rows * cols];
}

Matrix::Matrix(const std::string& file)
{
	std::ifstream infile(file.c_str());
	
	std::string line;
	
	std::vector<char> row;
	std::vector<std::vector<char> > rows;
	while (std::getline(infile, line))
	{
		std::istringstream iss(line);
		char a;
		while (iss >> a)
		{
			if (a != ' ')
			{
				row.push_back(a);
			}
		}
		
		if (row.size() == 0)
		{
			break;
		}
		
		rows.push_back(row);
		row.clear();
	}
		
	nRows = rows.size();
	if (nRows > 0)
	{
		nCols = rows[0].size();
	}
	std::cout << "Loaded matrix with size " << nRows << "x" << nCols << std::endl;
	
	data = new char[nRows * nCols];
	for (int i = 0; i < nRows; ++i)
	{
		for (int j = 0; j < nCols; ++j)
		{
			data[i * nCols + j] = rows[i][j];
		}
	}
}


Matrix::~Matrix()
{
	
}

MatrixFinder::MatrixFinder(const Matrix& data, const Matrix& pattern, const std::string& openclFile)
: mData(data)
, mPattern(pattern)
, mOpenclFile(openclFile)
{

}

cl::Program loadProgram(const cl::Context& context, const std::string& name, const std::string& options = std::string())
{
    std::cout << "Loading program " << name << std::endl;
  
    std::ifstream file(name.c_str());
    std::string prog(std::istreambuf_iterator<char>(file), (std::istreambuf_iterator<char>()));
    
    cl::Program::Sources source(1, std::make_pair(prog.c_str(), prog.length()));
    int err = 0;
    cl::Program program(context, source, &err);
    
    std::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
    
    err = program.build(devices, options.c_str());
    
    if (err < 0)
    {
        std::string log = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]);
        std::cout << log << std::endl;
    }
    
    return program;
}

cl::Buffer createBuffer(const cl::Context& context, const Matrix& matrix, bool writable = false, cl_int* err = 0)
{
    return cl::Buffer(context, writable ? CL_MEM_READ_WRITE : CL_MEM_READ_ONLY, matrix.nRows * matrix.nCols, NULL, err);
}

void writeBuffer(cl::CommandQueue& queue, cl::Buffer& buffer, const Matrix& matrix, cl_int* err = 0)
{
    cl_int result = queue.enqueueWriteBuffer(buffer, CL_FALSE, 0, matrix.nRows * matrix.nCols, matrix.data);
    if (err)
    {
	*err = result;
    }
}

void readBuffer(cl::CommandQueue& queue, cl::Buffer& buffer, const Matrix& matrix, cl_int* err = 0)
{
    cl_int result = queue.enqueueReadBuffer(buffer, CL_FALSE, 0, matrix.nRows * matrix.nCols, matrix.data);
    if (err)
    {
	*err = result;
    }
}

void MatrixFinder::setupOpenCL()
{
    cl_int err = 0;
    
    // NOTE: Change here if CPU is desired
    cl_device_type type = CL_DEVICE_TYPE_CPU;
        
    std::vector< cl::Platform > platformList;
    cl::Platform::get(&platformList);
    std::cout << "Platform count is: " << platformList.size() << std::endl;
    
    for (std::size_t i = 0; i < platformList.size(); ++i)
    {
        cl_context_properties cprops[3] = {
          CL_CONTEXT_PLATFORM,
          (cl_context_properties)(platformList[i])(),
          0
        };
        
        
        mContext = cl::Context(
          type,
          cprops,
          NULL,
          NULL,
          &err
        );
        if (err >= 0)
        {
		std::string platformVendor;
		platformList[i].getInfo((cl_platform_info)CL_PLATFORM_VENDOR, &platformVendor);
		std::cout << "Using platform " << platformVendor << "\n";
		break;
        }
    }
    
    if (err < 0)
    {
        std::cout << "Could not find suitable OpenCL device" << std::endl;
        exit(-err);
    }
    
    
    std::vector<cl::Device> devices = mContext.getInfo<CL_CONTEXT_DEVICES>();
    mDevice = devices[0];
    
    mProgram = loadProgram(mContext, mOpenclFile);
    mFindKernel = cl::Kernel(mProgram, "find");
}

std::vector< Coordinates > MatrixFinder::find()
{
	cl::Buffer dataBuffer = createBuffer(mContext, mData);
	cl::Buffer patternBuffer = createBuffer(mContext, mPattern);
	
	cl::CommandQueue queue(mContext, mDevice);
	
	Matrix indices(mData.nRows - mPattern.nRows + 1, mData.nCols - mPattern.nCols + 1);
	cl::Buffer indexBuffer = createBuffer(mContext, indices);
	
	// Setup and run the find kernel
	mFindKernel.setArg(0, dataBuffer);
	mFindKernel.setArg(1, patternBuffer);
	mFindKernel.setArg(2, mPattern.nRows);
	mFindKernel.setArg(3, mPattern.nCols);
	
	mFindKernel.setArg(4, indexBuffer);
	
	std::cout << "OpenCL initialized, running now" << std::endl;
	
	writeBuffer(queue, dataBuffer, mData);
	writeBuffer(queue, patternBuffer, mPattern);
	queue.enqueueNDRangeKernel(mFindKernel, cl::NullRange, cl::NDRange(indices.nRows, indices.nCols), cl::NullRange);
	readBuffer(queue, indexBuffer, indices);
	
	queue.finish();
	
	std::cout << "OpenCL run finished" << std::endl;
	
	std::vector<Coordinates> results;
	for (int i = 0; i < indices.nRows; ++i)
	{
		for (int j = 0; j < indices.nCols; ++j)
		{
			if (indices.data[i*indices.nCols + j])
			{
				Coordinates c = {i, j};
				results.push_back(c);
			}
		}
	}
	
	return results;
}

