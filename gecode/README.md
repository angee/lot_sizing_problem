### Discrete Lot Sizing Solver using Gecode library

This folder contains the C++ source code for a Constraint Programming (CP) 
solver for the discrete lot sizing problem using the open-source 
CP solver library [Gecode](http://gecode.org).

All the instructions here are for Linux operating systems.

#### Prerequisites
You will need `cmake` version 3.5 or higher, `make`, and 
naturally, a C++ compiler for C++11, for instance `g++`. 

You will need [Gecode](http://gecode.org) installed, in particular,
you will need Gecode's shared libraries in the `LD_LIBRARY_PATH`.
You can check the contents of the path with the command
`echo $LD_LIBRARY_PATH` in your terminal. 
Gecode is typically installed in `/usr/local/lib`.

#### Compilation
  
In the current directory, create the folder `build/` and change into it.
Then call cmake to create a `Makefile` and run `make` to compile the code.
Here are the commands for your terminal:

     mkdir build
     cd build/  
     cmake ..
     make 
    
This will create the executable `lotsizing` in the `build/` directory. 

#### Running the solver

After compilation, you can run the solver on one of the text-input format
benchmarks in [benchmarks/txt/](benchmarks/txt/). Here is an example
run:

     ./lotsizing ../../benchmarks/txt/instance_15timeslots_5types.txt   
     
Which will solve the respective instance with the standard settings.
There are 3 different branching strategies and 2 different search 
strategies: exact (complete) solving, or Large Neighbourhood Search (LNS).
You can checkout all the available options (and the default settings) 
by running:

     ./lotsizing --help     
          
#### Files     

This is a summary of the files in this folder:

    CMakeLists.txt               cmake file for compilation/building
    include/                     folder with header files    
    src/                         folder with source files
    README.md                    this README file 
    
The main problem class where variables are defined and constraints are 
posted is `lotsizing.h/cpp`, and there are two different custom branchers: 

     lotsizing_dynamic_greedy_brancher.h/cpp
     lotsizing_greedy_brancher.h/cpp  
     
The problem instance representation as well as the instance reader 
(that reads the text format description of the problem) are found in
`lotsizing_instance.h/cpp`. 