# Lot Sizing Problem

This repository contains solving approaches, examples and benchmarking tools for 
the discrete lot sizing problem. This is material for the Satalia-internal optimisation challenge.
 
The Lot sizing problem is described in detail as problem #58 in the 
CSPlib: http://www.csplib.org/Problems/prob058/


## Solving Approaches

#### Constraint Programming approach with Gecode

In the [gecode/](gecode/) folder you can find the C++ source code for a CP approach using the 
CP library solver [Gecode](http://gecode.org). It includes an exact approach, as well as a 
large neighbourhood search (LNS) approach, and it takes as input the examples in text format
found in the [examples/txt/](examples/txt/) folder.

For more details, check out the `README.md` in the [gecode/](gecode/) folder.

#### MIP and CP MiniZinc models

In the [minizinc/](minizinc/) folder there are two different problem models:
 
 (a) a Constraint Programming model in [minizinc/cp/lot_sizing_cp.mzn](minizinc/cp/lot_sizing_cp.mzn)
 
 (b) a Mixed Integer Programming model in [minizinc/mip/lot_sizing_mip.mzn](minizinc/mip/lot_sizing_mip.mzn)
 
You can use them to solve the problem with open-source [MiniZinc](http://minizinc.org) and one of the 
CP or MIP solvers that are shipped with it. The models take as input the `.dzn` files in the 
folder [examples/dzn/](examples/dzn). The CP model also comes with a solution validator.


## Problem examples

Problem instances can be found in the [examples/](examples/) folder.

## Running benchmarks

For running benchmarks on the CP approach with Gecode, check out the [benchmarking/](benchmarking/) folder. 