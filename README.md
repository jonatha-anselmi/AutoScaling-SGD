---
author: "Jonatha Anselmi"
date: "November 25, 2024"
title: "AutoScaling-SGD"
---

# AutoScaling-SGD

## Description

This project provides the code to support the numerical results published in [1]. Specifically:

- *autoscaling_kw.c* :
This file implements the proposed stochastic gradient descent algorithm for autoscaling; the full details are in Section III of [1]. 

- *autoscaling_mc.c* :
This file simulates the Markov chain that models dynamics induced by the auto-scaling algorithm introduced in Section II of [1] for a given $\theta$.

- *cost_function_gen.sh* :
This bash script generates the cost function (see Formula (3) in [1]) for increasing values of $\theta$ and saves it in cost_function_data.txt

- *cost_function_plot.m* :
This matlab script plots the cost function for increasing values of $\theta$

- *kw_plot.m* :
This script plots the cost function for increasing values of $\theta$ as in *cost_function_plot.m* and the sequence of $\theta$'s obtained by the proposed stochastic optimization algorithm. This provides the final picture (*cost_function_and_convergence.jpg*) that allows one to see whether or not the proposed algorithm makes $\theta$ converge to the minimum of the cost function given in *cost_function_data.txt*.

- *kw_plot_nonstationary.m* :
This script plots the sequence of $\theta$'s obtained by the proposed stochastic optimization algorithm when theta is updated every 100 or 1000 state transitions, thus without waiting for the underlying Markov chain "to reach stationarity". This alternative approach is for comparison purposes and provides the pictures *non_stationary1.eps* and *non_stationary2.eps*.

- *run.sh* :
Bash script to generate all the data.

## Usage

On Linux, to reproduce the simulation results in [1]:

 1.  Open a terminal and run *run.sh*
 2.  Open matlab and run *kw_plot.m* and  *kw_plot_nonstationary.m*


## References

[1] J. Anselmi, B. Gaujal, L.S. Rebuffi "Non-Stationary Gradient Descent for Optimal Auto-Scaling in Serverless Platforms", https://polaris.imag.fr/jonatha.anselmi/SGD_autoscaling.pdf
