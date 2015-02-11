cmake_minimum_required(VERSION 2.6)
project(waldo)
enable_testing()

find_package(OpenCL REQUIRED)

add_subdirectory(src)
