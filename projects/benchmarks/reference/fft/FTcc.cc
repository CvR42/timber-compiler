#include <time.h>
#include <stdio.h>
#include <complex.h>

typedef complex<double> Complex;

// CASE W
static const char Class   = 'W';
static const int MaxIterations = 6;
static const int Dim0=128;
static const int Dim1=128;
static const int Dim2=32;
static const Complex TestValue[] =
{
  Complex(5.673612178944E02,5.293246849175E02),
  Complex(5.631436885271E02,5.282149986629E02),
  Complex(5.594024089970E02,5.270996558037E02),
  Complex(5.560698047020E02,5.260027904925E02),
  Complex(5.530898991250E02,5.249400845633E02),
  Complex(5.504159734538E02,5.239212247086E02)
};

static const int TotalDim = Dim0 * Dim1 * Dim2;
typedef Complex Data3D[Dim0][Dim1][Dim2];
static Complex sums[MaxIterations];

static const int MaxDim = Dim0 >? Dim1 >? Dim2;
static Complex *omega;
static const int expMax = MaxIterations*(Dim0*Dim0 + Dim1*Dim1 + Dim2*Dim2);
static double ex[expMax+1];

static int indexmap[Dim0][Dim1][Dim2];

static Complex *temp1;
static Complex *temp2;

// RANDOM

static long long s; // seed
static long long m; // multiplier

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
{ // Skip the next N random numbers
  s*=powermod(m,N);
  s&=0x3fffffffffffLL;
}

inline static double rnd_double(void)
{
  s*=m;
  s&=0x3fffffffffffLL;
  return ((double) s / (double)0x400000000000LL);
}

static const double Pi    = 3.141592653589793238;
static const double alpha = 1.0E-6;

inline static int ILog2(const int n)
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
  // omega[t] = e^(2*Pi*i/2^t)

  int m = ILog2(MaxDim)+1;
  omega = new Complex[m];
  
  int k = 1;
  for (int i=0;i<m;i++)
    {
      double t = 2.0*Pi/k;
      omega[i] = Complex(cos(t),sin(t));
      k *= 2;
    }
}

inline static void FFT(const int is, const int N)
{
  // is = normal(1)/inverse(-1)
  // x.length and y.length must be 2^M
  // x is input. y is temporary vector
  // both x and y will be altered

  Complex *x = temp1;
  Complex *y = temp2;
  
  const int M = ILog2(N);

  for(int t=1;t<=M;t++)
    {
      int pM = 1 << (M-t);
      int pt = 1 << (t-1);
      
      Complex om1 = (is==-1) ? conj(omega[t]) : omega[t];
      Complex om = 1.0;

      for (int k=0;k<pt;k++)
	{
	  int id_from = k;
	  int id_to = k;

	  for (int j=0;j<pM;j++)
	    {
	      Complex x2 = om * x[id_from+N/2];

	      y[id_to] = x[id_from] + x2;
	      y[pt+id_to] = x[id_from] - x2;
	      id_from += pt;
	      id_to += 2*pt;
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

static void FFT3D(const int is, const Data3D u, Data3D v)
{
  // is = forward(1)/backward(-1)

  int i,j,k;

  // dim 1
  for(j=0;j<Dim1;j++)
    for(k=0;k<Dim2;k++)
      {
	for (i=0;i<Dim0;i++)
	  temp1[i] = u[i][j][k];
	FFT(is,Dim0);
	for (i=0;i<Dim0;i++)
	  v[i][j][k] = temp1[i];
      }
  // dim 2
  for(i=0;i<Dim0;i++)
    for(k=0;k<Dim2;k++)
      {
	for(j=0;j<Dim1;j++)
	  temp1[j] = v[i][j][k];
	FFT(is,Dim1);
	for (j=0;j<Dim1;j++)
	  v[i][j][k] = temp1[j];
      }
  // dim 3
  for(i=0;i<Dim0;i++)
    for(j=0;j<Dim1;j++)
      {
	for (k=0;k<Dim2;k++)
	  temp1[k] = v[i][j][k];
	FFT(is,Dim2);
	for (k=0;k<Dim2;k++)
	  v[i][j][k] = temp1[k];
      }
}

static void ComputeInitialConditions(Data3D u)
{
  const long GenerationSeed = 314159265LL;
  const long GenerationMult = 1220703125LL;
  
  for(int k=0;k<Dim2;k++)
    {
      rnd_seed(GenerationSeed,GenerationMult);
      rnd_skip(2*k*Dim1*Dim0);

      for (int j=0;j<Dim1;j++)
	for (int i=0;i<Dim0;i++)
	  u[i][j][k] = Complex(rnd_double(),rnd_double());
    }
}

static void ComputeIndexMap(void)
{
  for(int i=0;i<Dim0;i++)
    for(int j=0;j<Dim1;j++)
      for(int k=0;k<Dim2;k++)
	{
	  int r = (i+Dim0/2)%Dim0 - Dim0/2;
	  int s = (j+Dim1/2)%Dim1 - Dim1/2;
	  int t = (k+Dim2/2)%Dim2 - Dim2/2;
	  indexmap[i][j][k] = t*t + s*s + r*r;
	}
  
  ex[0] = 1.0;
  ex[1] = exp(-4.0*alpha*Pi*Pi);
  for (int i=2;i<=expMax;i++)
    ex[i] = ex[i-1] * ex[1];
}

static void Evolve(int iter, const Data3D v, Data3D w)
{
  
  iter++;

  for(int i=0;i<Dim0;i++)
    for(int j=0;j<Dim1;j++)
      for(int k=0;k<Dim2;k++)
	w[i][j][k] = v[i][j][k] * ex[iter*indexmap[i][j][k]];
}

static void CheckSum(const int iter, const Data3D u)
{
  Complex chk = Complex(0.0,0.0);
  
  for (int k=1;k<=1024;k++)
    chk += u[k%Dim0][3*k%Dim1][5*k%Dim2];

  sums[iter] = chk/(double) TotalDim;
}

static bool Verify(void)
{
  double err;
  const double epsilon = 1.0E-12;
  
  for(int i=0;i<MaxIterations;i++)
    {

      err = fabs((sums[i].real()-TestValue[i].real())/TestValue[i].real());
      if (err>=epsilon)
	return false;
      
      err = fabs((sums[i].imag()-TestValue[i].imag())/TestValue[i].imag());
      if (err>=epsilon)
	return false;
    }
  return true;
}

static Data3D u,v,w;

int main(void)
{
#if 0
  printf("NAS Parallel Benchmarks SPAR version - FT Benchmark\n");
  printf(" Size: %dx%dx%d (class %c)\n",Dim0,Dim1,Dim2,Class);
  printf("  Iterations:   %d\n",MaxIterations);
#endif

  // initialize temporary FFT vectors
  temp1 = new Complex [MaxDim];
  temp2 = new Complex [MaxDim];
  
  clock_t T1 = clock();
  
  ComputeIndexMap();
  
  ComputeInitialConditions(v);
  
  FFT_Init();
  
  FFT3D(1,v,u);
  
  for(int iter=0;iter<MaxIterations;iter++)
    {
      Evolve(iter,u,v);
      FFT3D(-1,v,w);
      CheckSum(iter,w);
    }

  clock_t T2 = clock();

  double TIME = (double)(T2-T1)/(CLOCKS_PER_SEC);
  
  bool passedVerification = Verify();

  double mops = 1.0E-6*TotalDim *
    (14.8157+7.19641*log((double)TotalDim) +
     5.23518+7.21113*log((double)TotalDim)) *
    MaxIterations/TIME;

#if 0
  printf("\n\n Benchmark Completed\n");
  printf(" Class           =             %c\n",Class);  
  printf(" Size            =             %d x %d x %d\n",
	 Dim0,Dim1,Dim2);  
  printf(" Iterations      =             %d\n",MaxIterations);
  printf(" Time in seconds =             %.2f\n",TIME);
  printf(" Mop/s total     =             %f\n",mops);
  printf(" Operation type  =             %s\n","floating point");
  printf(" Verification    =             %s\n",
 	 passedVerification ? "SUCCESSFUL" : "UNSUCCESSFUL");
  printf(" Version         =             %s\n","SIDNEY (GNU-C++)");
  printf(" Compile date    =             %s\n",__DATE__);
  printf("\n");
#endif

  return 0;
}
