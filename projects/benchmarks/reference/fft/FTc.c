
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

typedef __complex__ double Complex;
#define MAX(a,b) ((a)>(b) ? (a) : (b))

#define Class 'W'
#define MaxIterations 6
#define DIM0 128
#define DIM1 128
#define DIM2 32

static const Complex TestValue[MaxIterations] =
{
  567.3612178944+529.3246849175i,
  563.1436885271+528.2149986629i,
  559.4024089970+527.0996558037i,
  556.0698047020+526.0027904925i,
  553.0898991250+524.9400845633i,
  550.4159734538+523.9212247086i
};

#define TOTAL_DIM (DIM0 * DIM1 * DIM2)
#define MAX_DIM (MAX(DIM0,MAX(DIM1,DIM2)))
#define EXP_MAX (MaxIterations*(DIM0*DIM0 + DIM1*DIM1 + DIM2*DIM2))

typedef Complex Data3D[DIM0][DIM1][DIM2];

static Complex sums[MaxIterations];
static Complex *omega;
static double ex[EXP_MAX+1];
static int indexmap[DIM0][DIM1][DIM2];
static Complex *temp1;
static Complex *temp2;

// RANDOM

static long long s; /* seed       */
static long long m; /* multiplier */

static long long powermod(long long a, long long exponent)
{
  long long result = 1LL;

  while(exponent)
    {
      if (exponent&1)
	{
	  result *= a;
	  result &= 0x3fffffffffffLL;
	}
      a*=a;
      a&=0x3fffffffffffLL;
      exponent>>=1;
    }
  return result;
}
	
static void rnd_seed(const long long seed, const long long mult)
{
  s = seed;
  m = mult;
}

static void rnd_skip(const long long N)
{ 
  /* skip the next N random numbers */
  s*=powermod(m,N);
  s&=0x3fffffffffffLL;
}

__inline__ static double rnd_double(void)
{
  /* return a double x, 0 < x < 1 */
  s*=m;
  s&=0x3fffffffffffLL;
  return ((double) s / (double)0x400000000000LL);
}

static const double Pi    = 3.141592653589793238;
static const double alpha = 1.0E-6;

__inline__ static int ILog2(const int n)
{
  if (n<0x0002) return  0;
  if (n<0x0004) return  1;
  if (n<0x0008) return  2;
  if (n<0x0010) return  3;
  if (n<0x0020) return  4;
  if (n<0x0040) return  5;
  if (n<0x0080) return  6;
  if (n<0x0100) return  7;
  if (n<0x0200) return  8;
  if (n<0x0400) return  9;
  if (n<0x0800) return 10;
  if (n<0x1000) return 11;
  if (n<0x2000) return 12;
  if (n<0x4000) return 13;
  if (n<0x8000) return 14;
  return 15;
}

static void FFT_Init(void)
{
  int i, m, k;
  double t;
  
  // omega[t] = e^(2*Pi*i/2^t)
  m = ILog2(MAX_DIM)+1;
  omega = malloc(m*sizeof(Complex));
  
  k = 1;
  for(i=0;i<m;i++)
    {
      t = 2.0*Pi/k;
      omega[i] = cos(t)+(1.0i)*sin(t);
      k *= 2;
    }
}

__inline__ static void FFT(const int is, const int N)
{
  // is = normal(1)/inverse(-1)
  // x.length and y.length must be 2^M
  // x is input. y is temporary vector
  // both x and y will be altered
  int t;
  Complex *x = temp1;
  Complex *y = temp2;
  
  const int M = ILog2(N);

  for(t=1;t<=M;t++)
    {
      int pM,pt,k;
      Complex om,om1;

      pM = 1 << (M-t);
      pt = 1 << (t-1);
      
      om1 = (is==-1) ? ~omega[t] : omega[t];
      om = 1.0;

      for (k=0;k<pt;k++)
	{
	  int id_from, id_to, j;
	  id_from = id_to = k;
	  for (j=0;j<pM;j++)
	    {
	      Complex x2;
	      x2          = om * x[id_from+N/2];
	      y[   id_to] =      x[id_from    ] + x2;
	      y[pt+id_to] =      x[id_from    ] - x2;
	      id_from += pt;
	      id_to   += (2*pt);
	    }
	  om *= om1;
	}
      {
	Complex *swap = x;
	x = y;
	y = swap;
      }
    }
  temp1 = x;
  temp2 = y;
}

static void FFT3D(const int is, Data3D u, Data3D v)
{
  // is = forward(1)/backward(-1)

  int i,j,k;
  
  // dim 1
  for(j=0;j<DIM1;j++)
    for(k=0;k<DIM2;k++)
      {
	for (i=0;i<DIM0;i++)
	  temp1[i] = u[i][j][k];
	FFT(is,DIM0);
	for (i=0;i<DIM0;i++)
	  v[i][j][k] = temp1[i];
      }
  // dim 2
  for(i=0;i<DIM0;i++)
    for(k=0;k<DIM2;k++)
      {
	for(j=0;j<DIM1;j++)
	  temp1[j] = v[i][j][k];
	FFT(is,DIM1);
	for (j=0;j<DIM1;j++)
	  v[i][j][k] = temp1[j];
      }
  // dim 3
  for(i=0;i<DIM0;i++)
    for(j=0;j<DIM1;j++)
      {
	for (k=0;k<DIM2;k++)
	  temp1[k] = v[i][j][k];
	FFT(is,DIM2);
	for (k=0;k<DIM2;k++)
	  v[i][j][k] = temp1[k];
      }
}

static void ComputeInitialConditions(Data3D u)
{
  const long GenerationSeed = 314159265LL;
  const long GenerationMult = 1220703125LL;

  int k;

  for(k=0;k<DIM2;k++)
    {
      int i,j;

      rnd_seed(GenerationSeed,GenerationMult);
      rnd_skip(2*k*DIM1*DIM0);

      for (j=0;j<DIM1;j++)
	for (i=0;i<DIM0;i++)
	  u[i][j][k] = rnd_double() + (1.0i)*rnd_double();
    }
}

static void ComputeIndexMap(void)
{
  int i,j,k;

  for(i=0;i<DIM0;i++)
    for(j=0;j<DIM1;j++)
      for(k=0;k<DIM2;k++)
	{
	  int r = (i+DIM0/2)%DIM0 - DIM0/2;
	  int s = (j+DIM1/2)%DIM1 - DIM1/2;
	  int t = (k+DIM2/2)%DIM2 - DIM2/2;
	  indexmap[i][j][k] = t*t + s*s + r*r;
	}
  
  ex[0] = 1.0;
  ex[1] = exp(-4.0*alpha*Pi*Pi);
  for (i=2;i<=EXP_MAX;i++)
    ex[i] = ex[i-1] * ex[1];
}

static void Evolve(int iter, Data3D v, Data3D w)
{
  int i,j,k;
  iter++;

  for(i=0;i<DIM0;i++)
    for(j=0;j<DIM1;j++)
      for(k=0;k<DIM2;k++)
	w[i][j][k] = v[i][j][k] * ex[iter*indexmap[i][j][k]];
}

static void CheckSum(const int iter, Data3D u)
{
  int k;
  Complex chk = 0;
  
  for (k=1;k<=1024;k++)
      chk += u[k%DIM0][3*k%DIM1][5*k%DIM2];

  sums[iter] = chk/TOTAL_DIM;
}

static int Verify(void)
{
  int i;
  double err;
  const double epsilon = 1.0E-12;
  
  for(i=0;i<MaxIterations;i++)
    {

      err = fabs((__real__ sums[i] - __real__ TestValue[i])/
		 __real__ TestValue[i]);

      if (err>=epsilon)
	return 0;
      
      err = fabs((__imag__ sums[i] - __imag__ TestValue[i])/
		 __imag__ TestValue[i]);
      if (err>=epsilon)
	return 1;
    }
  return 1;
}

static Data3D u,v,w;

int main(void)
{

  double mops,TIME;
  int passedVerification;
  clock_t T1,T2;
  int iter;

#if 0
  printf("NAS Parallel Benchmarks SPAR version - FT Benchmark\n");
  printf(" Size: %dx%dx%d (class %c)\n",DIM0,DIM1,DIM2,Class);
  printf("  Iterations:   %d\n",MaxIterations);
#endif

  // initialize temporary FFT vectors
  temp1 = malloc(sizeof(Complex)*MAX_DIM);
  temp2 = malloc(sizeof(Complex)*MAX_DIM);
  
  T1 = clock();
  
  ComputeIndexMap();
  
  ComputeInitialConditions(v);
  
  FFT_Init();

  FFT3D(1,v,u);
  
  for(iter=0;iter<MaxIterations;iter++)
    {
      Evolve(iter,u,v);
      FFT3D(-1,v,w);
      CheckSum(iter,w);
    }

  T2 = clock();

  TIME = (double)(T2-T1)/(CLOCKS_PER_SEC);
  
  passedVerification = Verify();

  mops = 1.0E-6*TOTAL_DIM * 
    (14.8157+7.19641*log(TOTAL_DIM) +
     5.23518+7.21113*log(TOTAL_DIM)) *
    MaxIterations/TIME;

#if 0
  printf("\n\n Benchmark Completed\n");
  printf(" Class           =   %c\n",Class);
  printf(" Size            =   %d x %d x %d\n",DIM0,DIM1,DIM2);
  printf(" Iterations      =   %d\n",MaxIterations);
  printf(" Time in seconds =   %.2f\n",TIME);
  printf(" Mop/s total     =   %f\n",mops);
  printf(" Operation type  =   %s\n","floating point");
  printf(" Verification    =   %s\n",
 	 passedVerification ? "SUCCESSFUL" : "UNSUCCESSFUL");
  printf(" Version         =   %s\n","SIDNEY (GNU-C)");
  printf(" Compile date    =   %s\n",__DATE__);
  printf("\n");
#endif

  return 0;
}
