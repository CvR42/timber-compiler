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


package search; 
import jgfutil.*; 

public class JGFSearchBench extends SearchGame implements JGFSection3{

  private int size; 

  public void JGFsetsize(int size){
    this.size = size;
  }

  public void JGFinitialise(){
    reset();
    for (int i=0; i < startingMoves[size].length(); i++)
      makemove(startingMoves[size].charAt(i) - '0');
    emptyTT();
  }

  public void JGFapplication(){
    int result = solve();
  }

  public void JGFvalidate(){
    int i,works[];
    int ref[][] = 
    {
//{1048236,842,348,242,182,82,40,18,10,4,3,4,0,0,0,0,
//      0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//      {238095,363779,141507,124308,83203,48002,25572,12720,6520,      
//                3184,1580,784,415,177,98,29,22,10,4,0,2,0,0,0,0,0,0,0,0,0,0,0},
     {422,97347,184228,270877,218810,132097,72059,37601,18645,9200,4460,
      2230,1034,502,271,121,55,28,11,6,4,2,1,0,0,0,0,0,0,0,0,0},
     {0,1,9,2885,105101,339874,282934,156627,81700,40940,20244,10278,4797,2424,
      1159,535,246,139,62,28,11,11,3,0,3,0,0,0,0,0,0,0}
};
    
    works = new int[32];
    for (i=0; i<32; i++)   works[i] = 0;
    for (i=0; i<TRANSIZE; i++)  works[he[i] & 31]++;  

    for (i = 0; i < 32; i++){
      int error = works[i] - ref[size][i];
      if (error != 0){
	System.out.print("Validation failed for work count " + i);
	System.out.print("Computed value = " + works[i]);
	System.out.print("Reference value = " + ref[size][i]);
      }
    } 


  }

  public void JGFtidyup(){
    // Make sure large arrays are gc'd.
    ht = null; 
    he = null; 
    System.gc();  
  }

  public void JGFrun(int size){

  
    JGFInstrumentor.addTimer("Section3:AlphaBetaSearch:Run", "positions",size);

    JGFsetsize(size); 
    JGFinitialise(); 
    JGFapplication(); 
    JGFvalidate(); 
    JGFtidyup(); 


    JGFInstrumentor.addOpsToTimer("Section3:AlphaBetaSearch:Run", (double) nodes);
    JGFInstrumentor.printTimer("Section3:AlphaBetaSearch:Run"); 
  }
}



