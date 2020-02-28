cmake_minimum_required(VERSION 3.10)

# set the project name
project(NokiaTest)

include_directories(./)
# add the executable
add_executable(NokiaTest main.cpp table_processor.cpp)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)