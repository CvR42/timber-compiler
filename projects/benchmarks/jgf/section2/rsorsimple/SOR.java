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

package rsorsimple; 
import jgfutil.*; 

public class SOR
{
    public static double Gtotal = 0.0;

    public static final void SORrun(double omega, double G[][], int num_iterations)
    {
	final int M = G.length;
	final int N = G[0].length;

	final double omega_over_four = omega * 0.25;
	final double one_minus_omega = 1.0 - omega;
       
	JGFInstrumentor.startTimer("Section2:SOR:Kernel"); 

	for ( int p=0; p<num_iterations; p++ ) {
	    for( int j=1; j<N-1; j++ ){
		for( int i=1; i<M-1; i++ ){
		    G[i][j] = omega_over_four * (G[i-1][j] + G[i+1][j] +
			    G[i][j-1] + G[i][j+1]) + one_minus_omega * G[i][j];
		}
	    }
	}


	JGFInstrumentor.stopTimer("Section2:SOR:Kernel");

	for( int i=1; i<M-1; i++ ){
	    for( int j=1; j<N-1; j++ ){
		Gtotal += G[i][j];
	    }
	}               

    }
}
			
