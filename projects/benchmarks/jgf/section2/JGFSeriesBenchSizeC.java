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


import series.*; 
import jgfutil.*; 

public class JGFSeriesBenchSizeC{ 

  public static void main(String argv[]){

    JGFInstrumentor.printHeader(2,2);

    JGFSeriesBench se = new JGFSeriesBench(); 
    se.JGFrun(2);

  }
}
