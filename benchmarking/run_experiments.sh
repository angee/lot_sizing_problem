#!/usr/bin/env bash
# ==================================================================================================================== #
#
# NOTE: Run the tests in _this_ directory, because you will need the csv files in the regressionTestFiles/ directory
#       to compare the results of your test runs.
#
# ==================================================================================================================== #

CUR_DIR=`pwd`

# put here the path to your lotsizing build directory
PATH_TO_EXECUTABLE=${CUR_DIR}/../gecode/build/

# the executable of the solving approach
EXECUTABLE=${PATH_TO_EXECUTABLE}lotsizing

# ==================================================================================================================== #

# Specify what features we want to test in this benchmark run

declare -a search_options=("exact")
declare -a branching_options=("sdf-random" "static-greedy" "hybrid-greedy")
#declare -a model_options=("base" "redundant")         # this will probably be ignored
#declare -a seed_options=(1 7 11) 
declare -a lns_destruction_rates=(0.5 0.7 0.9) 

NUM_SEARCH=${#search_options[@]}            # number of search options
NUM_BRANCHING=${#branching_options[@]}      # number of branching options
#NUM_SEEDS=${#seeds_options[@]}              # number of seed options
NUM_LNS_RATE=${#lns_destruction_rates[@]}   # number of LNS destruction rates

TIME_OUT=7200000  # the timeout in milliseconds

# ==================================================================================================================== #

# edit here the pattern matching expression to narrow down the examples to run
for problemfile in ../examples/txt/*.txt; do
    
    for ((s=0;s<=$NUM_SEARCH-1;s++)); do
	search=${search_options[$s]}
	
	for ((b=0;b<=$NUM_BRANCHING-1;b++)); do	    
	    branching=${branching_options[$b]}
	    
	    # Solving via LNS
	    if [[ "${search}" == "lns" ]]
	    then
		echo "----- LNS -----"
		for ((r=0;r<=$NUM_LNS_RATE-1;r++)); do
		    lns_rate=${lns_destruction_rates[$r]}
		    solution_file=${problemfile}_${search}_${branching}_${lns_rate}.sol
		    #stats_file=${problemfile}_${search}_${branching}_${lns_rate}.stats
		    
		    echo "${EXECUTABLE} --time $TIME_OUT --search lns --relax ${lns_destruction_rates[$r]} --branching ${branching} ${problemfile} > ${solution_file}"
		    ${EXECUTABLE} --time $TIME_OUT --search lns --relax ${lns_destruction_rates[$r]} --branching ${branching} ${problemfile} > ${solution_file}
		done
		
	    # Solving exactly
	    else
		echo "----- EXACT -----"
		solution_file=${problemfile}_${search}_${branching}.sol
		echo "solution file = ${solution_file}"
		stats_file=${problemfile}_${search}_${branching}.stats
		
		echo "${EXECUTABLE} --time $TIME_OUT --search ${search} --branching ${branching} --file-sol ${solution_file} -file-stat ${stats_file} ${problemfile}"
		${EXECUTABLE} --time $TIME_OUT --search ${search} --branching ${branching} -file-sol ${solution_file} -file-stat ${stats_file} ${problemfile}
	    fi
	done
    done
    
done
