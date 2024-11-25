/* 
  Author: Jonatha Anselmi
  Date: November 30, 2023
  Description: This code implements the stochastic optimization algorithm proposed in [1] to the autoscaling problem discussed in that reference (implemented in autoscaling_mc.c). This algorithm is a gradient-descent Kiefer–Wolfowitz-type algorithm. The novelty of this algorithm with respect to standard algorithms consists in considering the transient behavior of the underlying Markov system.
  Variable names will follow the notation used in [1]. 
  Assumptions:
	- Poisson arrivals - lambda
	- independent exponentially distributed job sizes - mu
	- independent setup times - beta
	- independent expiration times - gamma
	- unitary server speed
  We want to investigate whether the algorithm under investigation drives theta to the theta that minimizes the cost function; see [1] for further details.
  
  [1] J. Anselmi, B. Gaujal, L.S. Rebuffi "Non-Stationary Gradient Descent for Optimal Auto-Scaling in Serverless Platforms"

  TO COMPILE:
  gcc -o autoscaling_kw autoscaling_kw.c -lm

  PARAMETERS: [#servers] [seed] [total_sim_time] [job arrival rate divided by #servers]

  TO EXECUTE:
  gcc -o autoscaling_kw autoscaling_kw.c -lm && ./autoscaling_kw 50 1 1000000000 0.15 2> as_kw_res.txt  
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
	
	double lambda, beta, mu, gamma, theta, theta_prev; 
	double U, Nd, rnd, rnd2, tmp;
	 
	double cost, cost_avg, w_rej, *w;
	double *cost_avg_vec; // average cost obtained for the 2*K MC simulations

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

	/* KW parameters */
	double a_n, d_n; // step-size sequence and discretization step
	double D, D_n, state_changes=-1.0, sc; // \Delta and 'Delta_n in paper's notation
	int n; // algorithm time-step
	double t=0.0, T;
	int k,K; // K = # of MC simulations for a given theta
	double f_r, f_l; // for the KW update rule

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


	/* "parameters for N=20,50" */
	w_rej=1e3;
	w[1]=1;  // idle-on
	w[2]=1.0;  // busy
	w[3]=5.0;  // initializing
	w[4]=500.0; // blocked jobs
	beta=0.1;
	gamma=0.01;

	if (argc!=6 && argc!=7 )
	{
		printf ("\nUsage:   %s [#servers] [seed] [total simulation time] [arrival_rate/#servers] [theta_init] [optional: #state_changes_between_theta_updates]\n", argv[0]);
		printf ("Example: %s   gcc -o %s %s.c -lm && ./%s 100 1 1000 0.15 1 2> res_kw.txt\n", argv[0], argv[0], argv[0], argv[0]);
		return -1;
	}

	/* Read input */
	N=atoi(argv[1]);
	seed=atoi(argv[2]);
	T=atof(argv[3]);
	lambda=atof(argv[4]);
	// set the initial theta
	theta=atof(argv[5]);
	if (argc==7) state_changes=atof(argv[6]);

	mu=1; // let this be fixed to one
	Nd = (double) N;

	U = (lambda + beta + mu + gamma)*Nd; // uniformization constant

	// set the initial state
	x[0]=N;
	x[1]=0;
	x[2]=0;
	x[3]=0;
	x[4]=0;

	for (i=0;i<=4;i++) { x_avg[i]=x[i]; }

	theta_prev=theta;

printf("############### Running KW ###############\n");

K=1; // K=1 resp. 2 for lambda=0.15 resp. 0.3
	if (lambda==0.15) K=1;
	if (lambda==0.3)  K=2;
D=2*1e7;
n=1;

cost_avg_vec = (double*) calloc (2*K+1, sizeof(double));

fprintf(stderr, "%f\t\n", theta);

// KW starts here
while (t<=T)
{
    D_n = D*(1.0+log(n));    // data_ToN_submitted_v1
    
   // test transient samples: "only simulate D_n state changes (in average)"
   // if (state_changes>0) D_n = U*state_changes; 
 
    J   = (int) (D_n/((double)K));

    a_n=10.0/pow((double) n, 1.0);
    if (state_changes>0) { a_n*=((double) state_changes)/(0.5*D); }
    d_n=1.0/pow((double) n, 2.0/3.0);
//    d_n=1.0/pow((double) n, 0.75);
 
//    printf("--------------------------------------\n");
//    printf("Testing theta(%.2f) +- delta_n(%.2f) = (%.3f,%.3f)...\n", theta, d_n, theta+d_n, MAX(theta-d_n,1.0));
 
    for (k=1;k<=2*K;k++)
    {
	srand48(seed+k);
	srand(seed+k);

	if (k<=K) theta=MIN(Nd,theta_prev+d_n); else theta=MAX(theta_prev-d_n,1.0);

	// set the initial state for the k-th simulation (used for the v1 submission to ToN)
/*	x[0]=N-floor(lambda*Nd);
	x[1]=floor(lambda*Nd);
	x[2]=0.0;
	x[3]=0.0;
	x[4]=0.0;*/
	sc+=1.0;
	cost_avg=0.0; sc=0.0;
	// simulate the underlying MC for J steps
	for (j=0;j<J;j++)
	{

		if (state_changes>0) { if (sc>=state_changes) break; }

#if LOG_LEVEL >= 1
for (i=0;i<=4;i++)
	fprintf(stderr, " %d ", x[i]);
fprintf(stderr, "\n");
#endif

		// integrity check
		cum_sum=0; for (i=1;i<=3;i++) cum_sum+=x[i]; 
		if (x[4]>x[3] || cum_sum>N)
		{
			printf("(seed=%d) Event %d: Integrity check failed !!!\n", seed, j);
			printf("policy=%d\n", policy);
			printf("x(k=%d,j=%d) = ", k,j);
			for (i=0;i<=4;i++)
			{
				printf(" %d ", x[i]);
			}
			printf("\n");
			printf("%.0f%%: (iter.: %d, D_n=%d) ", 100.0*t/T, n, J);
			printf("theta=%f cost=%f (a_n=%f,d_n=%f)\n", theta, (f_l+f_r)/2.0, a_n, d_n);
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
sc+=1.0;
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
sc+=1.0;
				if (x[4]>0) {x[2]++;x[3]--;x[4]--;}
				else {x[1]++;x[3]--;}
			} else dummy++;

		} else if (rnd<=lambda*Nd + beta*Nd + mu*Nd) {
			// server departure
			if (rnd<=lambda*Nd + beta*Nd + mu*((double)x[2]))
			{
sc+=1.0;
				if (x[4]>0) {x[4]--;}
				else {x[1]++;x[2]--;}

			} else dummy++;
		} else {
			// server expiration
			if (rnd<=lambda*Nd + beta*Nd + mu*Nd + gamma*((double)x[1]))
			{
sc+=1.0;
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

		cost_avg += cost;
		
	} // end loop in j - MC simulation 

	tmp=(double) j;
	cost_avg/=tmp;
	cost_avg_vec[k]=cost_avg;

//         printf("k=%d - theta=%f cost=%f (a_n=%f,d_n=%f)\n", k, theta, cost_avg, a_n, d_n);

    t+=j;

    } // end loop in k

    f_r=0.0; f_l=0.0;
    for (k=1;k<=K;k++)
    {
    	f_r+=cost_avg_vec[k];
    	f_l+=cost_avg_vec[k+K];
    }

    f_r/=((double)K);
    f_l/=((double)K);

    // KW update rule
    theta=MIN(MAX(theta_prev-a_n*( f_r-f_l )/(2.0*d_n),1.0), Nd);

    theta_prev=theta;

//    t+=2.0*D_n;

    if (n%1000==0) 
    {
        printf("%.0f%%: (iteration: %d, D_n=%d) ", 100.0*MIN(t,T)/T, n, J);
        printf("theta=\033[1;33m %.3f\033[0m cost=%f (a_n=%f,d_n=%f)",theta,(f_l+f_r)/2.0,a_n,d_n);
//	printf("f_r=%f \nf_l=%f \nf_r-f_l=%f \nd_n=%f\n", f_r, f_l, f_r-f_l, d_n);
        if (f_l>f_r) printf(" -> (%f)", f_l-f_r); else printf(" <- (%f)", f_l-f_r);
        printf("\n");
    }

#if LOG_LEVEL >= 1
getchar();
#endif

    fprintf(stderr, "%f\t\n", theta);

    n++;

} // KW end

	printf("############### KW finished ############### \n");

	return 1;
	
}


