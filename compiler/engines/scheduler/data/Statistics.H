/* $Id: Statistics.H,v 1.1 2000/10/19 21:09:28 andi Exp $ */

#ifndef __STATISTICS_H
#define __STATISTICS_H


#define SUCCESS 1.0
#define FAILURE 0.0

typedef class Statistics *PStatistics;

class Statistics
{
  public:
    Statistics (long seed = 1000);

    double Gaussian (double mean, double std_dev);
    double Erlang (double m, double mean);
    double Poisson (double mean);
    double Exponentially (double a);
    double Geometric (double p);
    double UniformContinious (double a, double b);
    double UniformDiscrete (double i, double j);
    double Bernoulli (double p);
    double Binomial (double n, double p);

    int Gaussian (int mean, double std_dev);

    double Sigma (double n, double p);

  private:
    double t;
};

#endif
