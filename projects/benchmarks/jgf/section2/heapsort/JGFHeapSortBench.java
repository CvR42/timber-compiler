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


package heapsort; 

import jgfutil.*;

public class JGFHeapSortBench extends NumericSortTest implements JGFSection2{ 

  private int size; 
  private int datasizes[]={1000000,5000000,25000000};

  public void JGFsetsize(int size){
    this.size = size;
  }

  public void JGFinitialise(){
    array_rows = datasizes[size];
    buildTestData();
  }
 
  public void JGFkernel(){
    Do(); 
  }

  public void JGFvalidate(){
    boolean error;

    error = false; 
    for (int i = 1; i < array_rows; i++){
      error = (TestArray[i] < TestArray[i-1]); 
      if (error){
	System.out.println("Validation failed");
	System.out.println("Item " + i + " = " + TestArray[i]); 
	System.out.println("Item " + (i-1) + " = " + TestArray[i-1]); 
	break;
      }
    }
  }


  public void JGFtidyup(){
    freeTestData(); 
  }  



  public void JGFrun(int size){


    JGFInstrumentor.addTimer("Section2:HeapSort:Kernel", "items",size);

    JGFsetsize(size); 
    JGFinitialise(); 
    JGFkernel(); 
    JGFvalidate(); 
    JGFtidyup(); 

     
    JGFInstrumentor.addOpsToTimer("Section2:HeapSort:Kernel", array_rows); 
    JGFInstrumentor.printTimer("Section2:HeapSort:Kernel"); 
  }
}
