cmake_minimum_required(VERSION 2.6)
project(waldo)

add_executable(waldo main.cpp)

install(TARGETS waldo RUNTIME DESTINATION bin)
