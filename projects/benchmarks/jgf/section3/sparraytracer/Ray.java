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
*                 Original version of this code by                        *
*            Florian Doyon (Florian.Doyon@sophia.inria.fr)                *
*              and  Wilfried Klauser (wklauser@acm.org)                   *
*                                                                         *
*      This version copyright (c) The University of Edinburgh, 1999.      *
*                         All rights reserved.                            *
*                                                                         *
**************************************************************************/


package sparraytracer; 


final public class Ray {
	public [double^3] P, D;

	public Ray([double^3] pnt, [double^3] dir) {
		P = pnt;
		D = Vec.normalize( dir );
	}

	public Ray() {
		P = [0.0,0.0,0.0];
		D = [0.0,0.0,0.0];
	}

	public [double^3] point(double t) {
		return P + D * t;
	}

	public String toString() {
		return "{[" + P[0] + "," + P[1] + "," + P[2] + "] -> [" + D[0] + "," + D[1] + "," + D[2] + "]}";
	}
}
