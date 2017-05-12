/* $Id: Statistics.C,v 1.1 2000/10/19 21:09:28 andi Exp $ */

#include <stdlib.h>
#include <math.h>

#include "Statistics.H"


Statistics::Statistics (long seed)
{
  srand (seed);
  t = 0.0;
}
  
  
//
//  Generates a Gaussian random variable with mean 'mean'
//  and standard deviation 'std_dev'.
//
int
Statistics::Gaussian (int mean, double std_dev)
{
  int val;

  if (mean == 0)
    return 0;

  val = (int) Gaussian ((double) mean, std_dev);
  if (val > 0)
    return val;
  else
    return 0;
}

double
Statistics::Gaussian (double mean, double std_dev)
{
  double x, v1, v2, r;

     /* must this be t == 0 ?! not t = 0.0 ? */
  if (t == 0)
  {
    do
    {
      v1 = 2.0 * drand48() - 1.0;
      v2 = 2.0 * drand48() - 1.0;
      r = v1 * v1 + v2 * v2;
    }
    while (r >= 1.0);

    r = sqrt ((-2.0 * log (r))/r);
    t = v2 * r;
    return (mean + v1 * r * std_dev);
  }
  else
  {
    x = t;
    t = 0.0;
    return (mean + x * std_dev);
  }
}


//
//  Generate an m-Erlang random variable with mean 'mean'.
//
double
Statistics::Erlang (double m, double mean)
{
  int i;
  double w;

  w = 1.0;

  for (i = 0; i < m; i++)
    w *= drand48();

  return ((-mean/m) * log(w));
}


//
//  Generates a Poisson random variable with mean 'mean'.
//
double
Statistics::Poisson (double mean)
{
  int i;
  double u, p, f;

  i = 0;
  f = p = exp (-mean);
  u = drand48();

  while (f < u)
  {
    p *= (mean/(i+1.0));
    f += p;
    i++;
  }

  return (double) i;
}


//
//  Generate an exponentially distributed random variable with parameter 'a'.
//
double
Statistics::Exponentially (double a)
{
  return (-log (drand48()) / a);
}
  

//
//  Generate a geometric random variable S = (0,1,2, ...) with parameter p.
//
double
Statistics::Geometric (double p)
{
  return (double) ((int) (log (drand48()) / log(1.0-p)));
}
  

//
//  Generate a continuous uniform random variable [a,b]
//
double
Statistics::UniformContinious (double a, double b)
{
  return (a + (b-a) * drand48());
}
  

//
//  Generate a discrete uniform random variable.                  *
//
double
Statistics::UniformDiscrete (double i, double j)
{
  j = (j - 1 + 1.0) * drand48();
  return (double) (i + j);
}
  

//
//  Generate a Bernoulli random variable with P[1] = p and P[0] = 1-p.
//
double
Statistics::Bernoulli (double p)
{
  if (drand48() <= p)
    return SUCCESS;
  else
    return FAILURE;
}


//
//  Generates a binomial random variable with parameters n and p.
//
double
Statistics::Binomial (double n, double p)
{
  int i;
  double u, q, f;
   
  n = (double) (int) n;
   
  i = 0;
  f = q = pow (1.0 - p, (double) n);
  u = drand48();
  while (f <= u)
  {
    q *= (p * (n - i)/((i + 1.0) * (1.0 - p)));
    f += q;
    i++;
  }
    
  return (double) i;
}

//
// Returns 0 or n.  The probability to return 0 is p.
//
double
Statistics::Sigma (double n, double p)
{
  if (drand48() < p)
    return 0.0;
  else
    return n;
}
