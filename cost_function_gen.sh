#!/bin/bash

gcc -o autoscaling_mc autoscaling_mc.c -lm

if [ -e cost_function_data.txt ]
then
	rm cost_function_data.txt
fi
N=50
#for theta in {0..36}
for theta in {0..50}
do
#	T=`echo "scale=2 ; 1 + $theta / 8" | bc`
#	T=`echo "scale=2 ; 0 + $theta / 8" | bc`
	T=`echo "scale=2 ; 0 + $theta" | bc`
	./autoscaling_mc $N 2 100000000 $T 2>> cost_function_data.txt

done
echo =============================
cat cost_function_data.txt
