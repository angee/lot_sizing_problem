### Benchmarking scripts 

This folder contains scripts for running benchmarks on the examples and the solving algorithms,
as well as for generating graphs.


#### Running benchmarks

The Bash script ` run_experiments.sh` will run benchmarks:
 
     ./run_experiments.sh
      
You need to make the script executable first, for instance by `chmod 777 run_experiments.sh`. 
Please modify the script to your needs (such as setting time limit, and/or which benchmarks to run). 

For each instance-solving approach combination, it will produce two files in the folder in which the
instances files are located. 
The first file is a `*.sol` file, that contains each found solution, 
its objective value and the time stamp (in ms) when it was found.
The second file is a `*.stats` file that contains solving 
statistics such as overall runtime, search tree size, etc.

#### Extracting solutions

The solving output written into the `*.sol` and `*.stats` files can be extracted into CSV files.
The scripts for extracting solutions are written in Python using Python 3.5. 
Create a Python environment and activate it, then execute the python script
(the first line is only necessary the very first time):

     virtualenv -p python3.5 venv       
     source venv/bin/activate    
     python solution_processing/extract_results.py -i <INPUT_FOLDER> -o results.csv

that will process all the `*.sol` and `*.stat` files in the folder `<INPUT_FOLDER>` and write 
the summary of results into `results.csv`. It will also write detailed results for each solution
file into `<INPUT_FOLDER>/<SOLUTION_FILE>.csv` that will contain each solution, objective value
and time stamp (when solution was found) in milliseconds.

 