/* 
  Author: Jonatha Anselmi
  Date: November 30, 2023
  Description: This code simulates the dynamics of the autoscaling algorithm introduced in [1]. Roughly speaking, given theta>0, at the moment of each job arrival, the autoscaling algorithm initializes (theta - x_{init-0} - x_{idle-on})^+ servers. 
  Variable names will follow the notation used in [1]. 
  Assumptions:
	- Poisson arrivals - lambda
	- independent exponentially distributed job sizes - mu
	- independent setup times - beta
	- independent expiration times - gamma
	- unitary server speed
	- theta >= 1

  [1]

  We want to investigate the impact of parameter theta, i.e., 
  the number of servers to activate whenever an incoming job finds no idle-on  server.

  TO COMPILE:
  gcc -o autoscaling_mc autoscaling_mc.c -lm

  PARAMETERS: [#servers] [seed] [#simulation steps] [theta]

  TO EXECUTE:
  gcc -o autoscaling_mc autoscaling_mc.c -lm && ./autoscaling_mc 20 1 100000000 2 2> as_res.txt  
  
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MAX(x,y)	(((x) > (y)) ? (x) : (y))
#define MIN(x,y)	(((x) < (y)) ? (x) : (y))

#define LOG_LEVEL 0

double  exponential(double lambda)
{
     return -log(drand48()) / lambda;
}

int main(int argc, char **argv)
{
	int J,N; 	// # events to simulate, # servers
	int seed, i,j, cum_sum, policy;
	int dummy=0;
	
	double lambda, beta, mu, gamma, theta; 
	double U, Nd, rnd, rnd2, tmp;
	 
	double cost, cost_avg=0.0, w_rej, *w;

  	/* x is the system state: 
  	   x[0] = # cold servers
  	   x[1] = # idle-on servers
  	   x[2] = # busy servers
  	   x[3] = # initializing servers
  	   x[4] = # blocked jobs, which are waiting in the central queue (or init-1 servers)
  	 */
	int *x;
	long int *x_avg;
	double *x_avg_d;

	x  = (int*) calloc (4+1, sizeof(int));
	x_avg  = (long int*) calloc (4+1, sizeof(long int));
	x_avg_d  = (double*) calloc (4+1, sizeof(double));

	/* cost function weights */
	w  = (double*) calloc (4+1, sizeof(double));

	/* "nice parameters for N=20,50" */
	w_rej=1e3;
	w[1]=1;  // idle-on
	w[2]=1.0;  // busy
	w[3]=5.0;  // initializing
	w[4]=50.0; // blocked jobs
	beta=0.5;
	gamma=0.1;


	/* "nice parameters for N=20,50" */
	w_rej=1e3;
	w[1]=1;  // idle-on
	w[2]=1.0;  // busy
	w[3]=5.0;  // initializing
	w[4]=500.0; // blocked jobs
//	w[4]=250.0; // blocked jobs, also ok but better 500.
	beta=0.1;
	gamma=0.01;

	if (argc!=5)
	{
		printf ("\nUsage:   %s [#servers] [seed] [#simulation steps] [theta]\n", argv[0]);
		printf ("Example: %s   gcc -o %s %s.c -lm && ./%s 100 1 100000 2 2> res.txt\n", argv[0], argv[0], argv[0], argv[0]);
		return -1;
	}

	N=atoi(argv[1]);
	seed=atoi(argv[2]);
	J=atoi(argv[3]);
	theta=atof(argv[4]);

	Nd = (double) N;

//	lambda=0.1;
	lambda=0.15;
	mu=1; // let this be fixed to one

	U = (lambda + beta + mu + gamma)*Nd; // uniformization constant

	srand48(seed);
	srand(seed);

	//set the initial state
	x[0]=N;
	x[1]=0;
	x[2]=0;
	x[3]=0;
	x[4]=0;

	for (i=0;i<=4;i++) { x_avg[i]=x[i]; }
	
	for (j=0;j<J;j++)
	{

#if LOG_LEVEL >= 1
for (i=0;i<=4;i++)
	fprintf(stderr, " %d ", x[i]);
fprintf(stderr, "\n");
#endif

		// Integrity check
		cum_sum=0; for (i=1;i<=3;i++) cum_sum+=x[i]; 
		if (x[4]>x[3] || cum_sum>N)
		{
			printf("(seed=%d) Event %d: Integrity check failed !!!\n", seed, j);
			printf("x(%d) = ", j);
			for (i=0;i<=4;i++)
			{
				printf(" %d ", x[i]);
			}
			printf("\n");
			return -1;
		}

		rnd=drand48()*U;

		if (rnd<=lambda*Nd) {
			// job arrival
			double var=drand48();
			int ind1 = ((var - theta + floor(theta)) >= 0);
			int ind2 = ((var - theta + floor(theta)) < 0);

			policy = MIN(MAX(floor(theta)   - x[1] - (x[3]-x[4]),0), N-x[1]-x[2]-x[3]-1) * ind1;			
			policy+= MIN(MAX(floor(theta)+1 - x[1] - (x[3]-x[4]),0), N-x[1]-x[2]-x[3]-1) * ind2;

		  if (x[1]>0) {
		    //no auto-scaling
		  	x[1]--; // idle-on
		  	x[2]++; // busy
		  } else {
		    //yes auto-scaling
			if (policy>=0) {
				if (x[1]>0) {
					x[0]-=(policy+1); // cold
					x[1]--;           // idle-on
					x[2]++;           // busy
					x[3]+=(policy+1); // init (both 0 and 1)
				} else {
					x[0]-=(policy+1); // cold
					x[3]+=(policy+1); // init (both 0 and 1)
					x[4]+=1;  		  // init-1		
				}
			}

#if LOG_LEVEL >= 1
fprintf(stderr, "---> policy=%d (theta=%.2f)\n", policy, theta);
getchar();
#endif

  		  }

		} else if (rnd<=lambda*Nd + beta*Nd) {
			// server initialization
			if (rnd<=lambda*Nd + beta*((double)x[3]))
			{
				if (x[4]>0) {x[2]++;x[3]--;x[4]--;}
				else {x[1]++;x[3]--;}
			} else dummy++;

		} else if (rnd<=lambda*Nd + beta*Nd + mu*Nd) {
			// server departure
			if (rnd<=lambda*Nd + beta*Nd + mu*((double)x[2]))
			{
				if (x[4]>0) {x[4]--;}
				else {x[1]++;x[2]--;}

			} else dummy++;
		} else {
			// server expiration
			if (rnd<=lambda*Nd + beta*Nd + mu*Nd + gamma*((double)x[1]))
			{
					x[1]--;
					x[0]++;
			} else dummy++;
		}

		for (i=0;i<=4;i++) { x_avg[i] += x[i]; }

		/* Compute cost function in x */
		cost=0.0;
		for (i=1;i<=4;i++)
		{
			tmp=(double) x[i];
			cost+=tmp*w[i];
		}

		// add rejection cost
		if (x[2]+x[4]==N) cost+=w_rej;

//		cost-=w[3]*x[4];
		cost_avg += cost;
		
	}

	tmp=(double) J;
	cost_avg/=tmp;
	for (i=0;i<=4;i++) x_avg_d[i] = ((double) x_avg[i]) / tmp;

	printf("############### theta=%.2f ############### \n", theta);

	printf("Average cost: \033[1;33m %.3f\033[0m \n", cost_avg);

	printf("Average state: \n");
	printf("x[0] = %.5f; # cold servers\n",  x_avg_d[0]);
	printf("x[1] = %.5f; # idle-on servers\n",  x_avg_d[1]);
	printf("x[2] = %.5f; # busy servers\n",  x_avg_d[2]);
	printf("x[3] = %.5f; # initializing servers\n",  x_avg_d[3]);
	printf("x[4] = %.5f; # blocked jobs\n",  x_avg_d[4]);

	printf("dummy transitions = %d - %.2f%%\n",  dummy, 100.0*((double)dummy)/((double)J));

	fprintf(stderr, "%f\t%f\n", theta, cost_avg);

	return 1;
	
}



