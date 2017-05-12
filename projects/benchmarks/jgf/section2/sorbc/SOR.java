/**************************************************************************
*                                                                         *
*             Java Grande Forum Benchmark Suite - Version 2.0             *
*                                                                         *
*                            produced by                                  *
*                                                                         *
*                  Java Grande Benchmarking Project                       *
*                                                                         *
*                                at                                       *
*                                                                         *
*                Edinburgh Parallel Computing Centre                      *
*                                                                         *
*                email: epcc-javagrande@epcc.ed.ac.uk                     *
*                                                                         *
*      adapted from SciMark 2.0, author Roldan Pozo (pozo@cam.nist.gov)   *
*                                                                         *
*      This version copyright (c) The University of Edinburgh, 1999.      *
*                         All rights reserved.                            *
*                                                                         *
**************************************************************************/

package sorbc; 
import jgfutil.*; 

public class SOR
{

  public static double Gtotal = 0.0;

	public static final void SORrun(double omega, double G[][], int num_iterations)
	{
		int M = G.length;
		int N = G[0].length;

		double omega_over_four = omega * 0.25;
		double one_minus_omega = 1.0 - omega;
               

		// update interior points
		//
		int Mm1 = M-1;
		int Nm1 = N-1;

                JGFInstrumentor.startTimer("Section2:SOR:Kernel"); 

		for (int p=0; p<num_iterations; p++)
		{
			int M1 = Math.min( Mm1, G.length-1 );
			for (int i=1; i<M1; i++)
			{
				 double [] Gi = G[i];
				 double [] Gim1 = G[i-1];
				 double [] Gip1 = G[i+1];
				int N1 = Math.min(
				    Nm1,
				    Math.min(
				        Gi.length-1,
					Math.min( Gim1.length-1, Gip1.length-1 )
				    )
				);
				for (int j=1; j<N1; j++)
					(<$boundscheck=false$> Gi[j]) = omega_over_four * ((<$boundscheck=false$>Gim1[j]) + (<$boundscheck=false$> Gip1[j]) + (<$boundscheck=false$> Gi[j-1] )
								+ (<$boundscheck=false$> Gi[j+1])) + one_minus_omega * (<$boundscheck=false$> Gi[j] );
				if( N1<Nm1 ){
				    throw new ArrayIndexOutOfBoundsException();
				}
			}
			if( M1<Mm1 ){
			    throw new ArrayIndexOutOfBoundsException();
			}
		}

 
                JGFInstrumentor.stopTimer("Section2:SOR:Kernel");

                for (int i=1; i<Nm1; i++) {
                 for (int j=1; j<Nm1; j++) {
                  Gtotal += G[i][j];
                 }
                }               
 
	}
}
			
