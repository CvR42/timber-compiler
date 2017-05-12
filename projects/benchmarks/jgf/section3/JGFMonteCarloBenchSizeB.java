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
*                                                                         *
*      This version copyright (c) The University of Edinburgh, 1999.      *
*                         All rights reserved.                            *
*                                                                         *
**************************************************************************/


import montecarlo.*;

import jgfutil.*;

public class JGFMonteCarloBenchSizeB{ 

  public static void main(String argv[]){

    JGFInstrumentor.printHeader(3,1);

    JGFMonteCarloBench mc = new JGFMonteCarloBench(); 
    mc.JGFrun(1);
 
  }
}
 
