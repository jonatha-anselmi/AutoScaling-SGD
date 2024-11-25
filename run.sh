#!/bin/bash

if ls as_kw_res*.txt 1> /dev/null 2>&1; then
	rm as_kw_res*.txt
fi

gcc -o autoscaling_kw autoscaling_kw.c -lm

for theta in 1 10
do

	########################################################
	# Run the proposed algorithm.
	# Stationary samples of the underlying Markov chain
	########################################################

	./autoscaling_kw 50 1 1000000000 0.15 $theta 2> as_kw_res_theta${theta}_L015.txt
	./autoscaling_kw 50 1 1000000000 0.15 $theta 2> as_kw_res_theta${theta}_L015.txt

	./autoscaling_kw 50 1 1000000000 0.3  $theta 2> as_kw_res_theta${theta}N50.txt
	./autoscaling_kw 50 1 1000000000 0.3  $theta 2> as_kw_res_theta${theta}N50.txt


	########################################################
	# Run an alternative version.
	# Non stationary samples of the underlying Markov chain (100 and 1000 samples)
	########################################################

	./autoscaling_kw 50 1 1000000000 0.15 $theta 100  2> as_kw_res_theta${theta}_L015_100.txt
	./autoscaling_kw 50 1 1000000000 0.15 $theta 1000 2> as_kw_res_theta${theta}_L015_1000.txt

	./autoscaling_kw 50 1 1000000000 0.3 $theta 100  2> as_kw_res_theta${theta}_L03_100.txt
	./autoscaling_kw 50 1 1000000000 0.3 $theta 1000 2> as_kw_res_theta${theta}_L03_1000.txt

done

# Generate cost function
#./cost_function_gen.sh

