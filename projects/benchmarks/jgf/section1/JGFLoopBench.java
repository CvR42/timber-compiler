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
*    Original version of this code by DHPC Group, Univ. of Adelaide       *
*                    See copyright notice below.                          *
*                                                                         *
*                                                                         *
*      This version copyright (c) The University of Edinburgh, 1999.      *
*                         All rights reserved.                            *
*                                                                         *
**************************************************************************/

/*
 *  Copyright (C) 1998, University of Adelaide, under its participation
 *  in the Advanced Computational Systems Cooperative Research Centre
 *  Agreement.
 *
 *  THIS SOFTWARE IS MADE AVAILABLE, AS IS, AND THE UNIVERSITY
 *  OF ADELAIDE DOES NOT MAKE ANY WARRANTY ABOUT THE SOFTWARE, ITS
 *  PERFORMANCE, ITS MERCHANTABILITY OR FITNESS FOR ANY PARTICULAR
 *  USE, FREEDOM FROM ANY COMPUTER DISEASES OR ITS CONFORMITY TO ANY
 *  SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND PERFORMANCE OF
 *  THE SOFTWARE IS WITH THE USER.
 *
 *  Copyright of the software and supporting documentation is owned by the
 *  University of Adelaide, and free access is hereby granted as a license
 *  to use this software, copy this software and prepare derivative works
 *  based upon this software.  However, any distribution of this software
 *  source code or supporting documentation or derivative works (source
 *  code and supporting documentation) must include this copyright notice
 *  and acknowledge the University of Adelaide.
 *
 *
 *  Developed by: Distributed High Performance Computing (DHPC) Group
 *                Department of Computer Science
 *                The University of Adelaide
 *                South Australia 5005
 *                Tel +61 8 8303 4519, Fax +61 8 8303 4366
 *                http://dhpc.adelaide.edu.au
 *  Last Modified: 26 January 1999
 *
 *
 ***************************************************************************
 *  			Java Benchmarks    
 *                                                                     
 *  These benchmarks complement the Java Grande Benchmarks and utilise 
 *  the timing and result reporting framework provided with them.      
 *  
 *  Section1a includes extra low level benchmarks, Section2a includes extra
 *  kernel benchmarks and util includes commonly used code such as for 
 *  printing header information
 *
 *  Jesudas Mathew (jm@cs.adelaide.edu.au)
 ***************************************************************************
 *
 *  The tests in this class measure the performance of looping constructs
 */


import jgfutil.*; 

public class JGFLoopBench implements JGFSection1{


  private static final int INITSIZE = 10000;
  private static final int MAXSIZE = 1000000000;
  private static final double TARGETTIME = 10.0; 
  

  public void JGFrun(){

    int i,size;
    double time; 

    // Empty for loop
    JGFInstrumentor.addTimer("Section1:Loop:For", "iterations");
    time = 0.0; 
    size = INITSIZE; 
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Loop:For"); 
      JGFInstrumentor.startTimer("Section1:Loop:For"); 
      for (i=0;i<size;i++) {
      }
      JGFInstrumentor.stopTimer("Section1:Loop:For"); 
      if (i>size) System.out.println(i);  
      time = JGFInstrumentor.readTimer("Section1:Loop:For"); 
      JGFInstrumentor.addOpsToTimer("Section1:Loop:For", (double) size);  
      size *=2; 
    }
    JGFInstrumentor.printperfTimer("Section1:Loop:For");
 


    // Reverse iterating for loop
    JGFInstrumentor.addTimer("Section1:Loop:ReverseFor", "iterations");
    time = 0.0; 
    size = INITSIZE; 
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Loop:ReverseFor"); 
      JGFInstrumentor.startTimer("Section1:Loop:ReverseFor"); 
      for (i=size;i>0;i--) {
      }
      JGFInstrumentor.stopTimer("Section1:Loop:ReverseFor");
      if (i>0) System.out.println(i);  
      time = JGFInstrumentor.readTimer("Section1:Loop:ReverseFor"); 
      JGFInstrumentor.addOpsToTimer("Section1:Loop:ReverseFor", (double) size);  
      size *=2; 
    }
    JGFInstrumentor.printperfTimer("Section1:Loop:ReverseFor"); 
  

    // Empty while loop
    JGFInstrumentor.addTimer("Section1:Loop:While", "iterations");
    time = 0.0; 
    size = INITSIZE; 
    
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Loop:While"); 
      JGFInstrumentor.startTimer("Section1:Loop:While");
      i=0;
      while (i++<size) {
      }
      JGFInstrumentor.stopTimer("Section1:Loop:While");
      if (i<size) System.out.println(i); 
      time = JGFInstrumentor.readTimer("Section1:Loop:While"); 
      JGFInstrumentor.addOpsToTimer("Section1:Loop:While", (double) size);  
      size *=2; 
    }
    JGFInstrumentor.printperfTimer("Section1:Loop:While"); 
  }
  
public static void main (String argv[]){

    JGFInstrumentor.printHeader(1,0);

    JGFLoopBench lb = new JGFLoopBench(); 
    lb.JGFrun();
    
  }
}


