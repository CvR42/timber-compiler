public final class FTjava {

    // class W
    static final char Class = 'W';
    static final int MaxIterations = 6;
    static final int Dim0 = 128;
    static final int Dim1 = 128;
    static final int Dim2 = 32;
    static final int MaxDim = 128;
    static final int TotalDim = Dim0 * Dim1 * Dim2;

    static final double TestValuere [] =
    {
	567.3612178944,563.1436885271,559.4024089970,
	556.0698047020,553.0898991250,550.4159734538
    };
    static final double TestValueim [] =
    {
	529.3246849175,528.2149986629,527.0996558037,
	526.0027904925,524.9400845633,523.9212247086
    };

    static double temp1re[] = new double[MaxDim];
    static double temp1im[] = new double[MaxDim];
    static double temp2re[] = new double[MaxDim];
    static double temp2im[] = new double[MaxDim];

    static double omegare[];
    static double omegaim[];

    static final double ure[][][] = new double[Dim0][Dim1][Dim2];
    static final double uim[][][] = new double[Dim0][Dim1][Dim2];
    static final double vre[][][] = new double[Dim0][Dim1][Dim2];
    static final double vim[][][] = new double[Dim0][Dim1][Dim2];
    static final double wre[][][] = new double[Dim0][Dim1][Dim2];
    static final double wim[][][] = new double[Dim0][Dim1][Dim2];

    static final int indexmap[][][] = new int[Dim0][Dim1][Dim2];
    static final int expMax = MaxIterations * (Dim0*Dim0 + Dim1*Dim1 + Dim2*Dim2);
    static final double ex[] = new double[expMax+1];
    static final double sumsre[] = new double[MaxIterations];
    static final double sumsim[] = new double[MaxIterations];

    static final double Pi    = 3.141592653589793238;
    static final double alpha = 1.0E-6;

    static void ComputeIndexMap()
    {
	for(int i=0;i<Dim0;i++)
	    for(int j=0;j<Dim1;j++)
		for(int k=0;k<Dim2;k++) {
		    int r = (i+Dim0/2)%Dim0 - Dim0/2;
		    int s = (j+Dim1/2)%Dim1 - Dim1/2;
		    int t = (k+Dim2/2)%Dim2 - Dim2/2;
		    indexmap[i][j][k] = t*t + s*s + r*r;
		}
	
	ex[0] = 1.0;
	ex[1] = Math.exp(-4.0*alpha*Pi*Pi);
	for (int i=2; i<expMax; i++)
	    ex[i] = ex[i-1] * ex[1];
    }

    static long powermod(long a, long exponent)
    {
	long result = 1L;
	
	while(exponent!=0) {
	    if ((exponent&1)!=0) {
		result *= a;
		result &= 0x3fffffffffffL;
	    }
	    a*=a;
	    a&=0x3fffffffffffL;
	    exponent>>=1;
	}
	return result;
    }

    static long s,m;

    static void rnd_seed(long seed, long mult)
    {
	s = seed;
	m = mult;
    }
    
    static void rnd_skip(long N)
    { // Skip the next N random numbers
	s*=powermod(m,N);
	s&=0x3fffffffffffL;
    }
    
    static double rnd_double()
    {
	s*=m;
	s&=0x3fffffffffffL;
	return ((double) s / (double)0x400000000000L);
    }

    static void ComputeInitialConditions(double ure[][][], double uim[][][])
    {
	final long GenerationSeed = 314159265L;
	final long GenerationMult = 1220703125L;
	
	for(int k=0;k<Dim2;k++) {
	    rnd_seed(GenerationSeed,GenerationMult);
	    rnd_skip(2*k*Dim1*Dim0);
	    
	    for (int j=0;j<Dim1;j++)
		for (int i=0;i<Dim0;i++) {
		    ure[i][j][k] = rnd_double();
		    uim[i][j][k] = rnd_double();
		}
	}
    }

    static int ILog2(int n)
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
    
    static void FFT_Init()
    {
	// omega[t] = e^(2*Pi*i/2^t)
	int m = ILog2(MaxDim)+1;
	omegare = new double[m];
	omegaim = new double[m];
	
	int k = 1;
	for (int i=0;i<m;i++) {
	    double t = 2.0*Pi/k;
	    omegare[i] = Math.cos(t);
	    omegaim[i] = Math.sin(t);
	    k *= 2;
	}
    }

    static void Evolve(int iter, double vre[][][], double vim[][][], double wre[][][], double wim[][][])
    {
	iter++;
	
	for(int i=0;i<Dim0;i++)
	    for(int j=0;j<Dim1;j++)
		for(int k=0;k<Dim2;k++) {
		    double multiply = ex[iter*indexmap[i][j][k]];
		    wre[i][j][k] = vre[i][j][k] * multiply;
		    wim[i][j][k] = vim[i][j][k] * multiply;
		}
    }
    
    static void CheckSum(final int iter, double ure[][][], double uim[][][])
    {
	double chkre = 0.0;
	double chkim = 0.0;
	
	for (int k=1;k<=1024;k++) {
	    int q = (  k)%Dim0;
	    int r = (3*k)%Dim1;
	    int s = (5*k)%Dim2;
	    chkre += ure[q][r][s];
	    chkim += uim[q][r][s];
	}

	sumsre[iter] = chkre/TotalDim;
	sumsim[iter] = chkim/TotalDim;
    }

    static void FFT(final int is, final int N)
    {
	// is = normal(1)/inverse(-1)
	// x.length and y.length must be 2^M
	// x is input. y is temporary vector
	// both x and y will be altered

	double xre[] = temp1re;
	double xim[] = temp1im;
	double yre[] = temp2re;
	double yim[] = temp2im;
	
	int M = ILog2(N);

	for(int t=1;t<=M;t++) {
	    int pM = 1 << (M-t);
	    int pt = 1 << (t-1);

	    double omre,omim,om1re,om1im;
	    
	    if (is==-1) {
		om1re =  omegare[t];
		om1im = -omegaim[t];
	    }
	    else {
		om1re =  omegare[t];
		om1im =  omegaim[t];
	    }

	    omre = 1.0;
	    omim = 0.0;
	    
	    for (int k=0;k<pt;k++) {
		int id_from = k;
		int id_to = k;

		for (int j=0;j<pM;j++) {
		    double x2re = omre * xre[id_from+N/2] - omim * xim[id_from+N/2];
		    double x2im = omre * xim[id_from+N/2] + omim * xre[id_from+N/2];

		    yre[   id_to] = xre[id_from] + x2re;
		    yim[   id_to] = xim[id_from] + x2im;
		    yre[pt+id_to] = xre[id_from] - x2re;
		    yim[pt+id_to] = xim[id_from] - x2im;
		    id_from += pt;
		    id_to += 2*pt;
		}

		double new_omre = omre * om1re - omim * om1im;
		omim = omre * om1im + omim * om1re;
		omre = new_omre;

	    }
	    {
		double swap [];
		swap = xre; xre = yre; yre = swap;
		swap = xim; xim = yim; yim = swap;
	    }
	}
	temp1re = xre;
	temp1im = xim;
	temp2re = yre;
	temp2im = yim;
    }
    
    static void FFT3D(final int is, double ure[][][], double uim[][][], double vre[][][], double vim[][][])
    {
	// is = forward(1)/backward(-1)
	
	int i,j,k;

	// dim 1
	for(j=0;j<Dim1;j++)
	    for(k=0;k<Dim2;k++) {
		for (i=0;i<Dim0;i++) {
		    temp1re[i] = ure[i][j][k];
		    temp1im[i] = uim[i][j][k];
		}
		FFT(is,Dim0);
		for (i=0;i<Dim0;i++) {
		    vre[i][j][k] = temp1re[i];
		    vim[i][j][k] = temp1im[i];
		}
	    }
	// dim 2
	for(i=0;i<Dim0;i++)
	    for(k=0;k<Dim2;k++) {
		for(j=0;j<Dim1;j++) {
		    temp1re[j] = vre[i][j][k];
		    temp1im[j] = vim[i][j][k];
		}
		FFT(is,Dim1);
		for (j=0;j<Dim1;j++) {
		    vre[i][j][k] = temp1re[j];
		    vim[i][j][k] = temp1im[j];
		}
	    }
	// dim 3
	for(i=0;i<Dim0;i++)
	    for(j=0;j<Dim1;j++) {
		for (k=0;k<Dim2;k++) {
		    temp1re[k] = vre[i][j][k];
		    temp1im[k] = vim[i][j][k];
		}
		FFT(is,Dim2);
		for (k=0;k<Dim2;k++) {
		    vre[i][j][k] = temp1re[k];
		    vim[i][j][k] = temp1im[k];
		}
	    }
    }
    
    static boolean Verify()
    {
	double err;
	final double epsilon = 1.0E-12;
	
	for(int i=0;i<MaxIterations;i++) {
	    
	    err = Math.abs((sumsre[i]-TestValuere[i])/TestValuere[i]);
	    if (err>=epsilon)
		return false;
	    
	    err = Math.abs((sumsim[i]-TestValueim[i])/TestValueim[i]);
	    if (err>=epsilon)
		return false;
	}
	return true;
    }

    public static void main(String args[])
    {
	if( false ){
	    System.out.println("NAS Parallel Benchmarks Java version - FT Benchmark");
	    System.out.println(" Size: "+Dim0+"x"+Dim1+"x"+Dim2+" (class "+Class+")");
	    System.out.println("  Iterations:   "+MaxIterations);
	}
	
	long T1 = System.currentTimeMillis();

	ComputeIndexMap();

	ComputeInitialConditions(vre,vim);

	FFT_Init();
	FFT3D(1,vre,vim,ure,uim);

	for(int iter=0;iter<MaxIterations;iter++) {
	    Evolve(iter,ure,uim,vre,vim);
	    FFT3D(-1,vre,vim,wre,wim);
	    CheckSum(iter,wre,wim);
	}
	long T2 = System.currentTimeMillis();

	double TIME = 0.001*(T2-T1);

	boolean passedVerification = Verify();

	double mops = 1.0E-6*TotalDim *
	    (14.8157+7.19641*Math.log((double)TotalDim) +
	     5.23518+7.21113*Math.log((double)TotalDim)) *
	    MaxIterations/TIME;

	if( false ){
	    System.out.println("\n\n Benchmark Completed");
	    System.out.println(" Class           =             " + Class);  
	    System.out.println(" Size            =             " + Dim0 + " x " + Dim1 + " x " + Dim2);
	    System.out.println(" Iterations      =             " + MaxIterations);
	    System.out.println(" Time in seconds =             " + TIME);
	    System.out.println(" Mop/s total     =             " + mops);
	    System.out.println(" Operation type  =             " + "floating point");
	    System.out.println(" Verification    =             " +
			       (passedVerification ? "SUCCESSFUL" : "UNSUCCESSFUL"));
	    System.out.println(" Version         =             "+"SIDNEY (Java)");
	}
    }
}
