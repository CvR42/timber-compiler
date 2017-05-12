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


import crypt.*;
import series.*; 
import heapsort.*; 
import lufact.*; 
import sor.*;
import fft.*;
import sparsematmult.*;
import jgfutil.*; 

public class JGFAllSizeC{

  public static void main(String argv[]){
   
    int size = 2; 

    JGFInstrumentor.printHeader(2,size);

    JGFCryptBench cb = new JGFCryptBench();
    cb.JGFrun(size);    

    JGFSeriesBench se = new JGFSeriesBench(); 
    se.JGFrun(size);

    JGFLUFactBench lub = new JGFLUFactBench();
    lub.JGFrun(size);    

    JGFHeapSortBench hb = new JGFHeapSortBench();
    hb.JGFrun(size);    

    JGFFFTBench fft = new JGFFFTBench(); 
    fft.JGFrun(size);
   
    JGFSORBench jb = new JGFSORBench(); 
    jb.JGFrun(size);
   
    JGFSparseMatmultBench smm = new JGFSparseMatmultBench(); 
    smm.JGFrun(size);
    
    
  }
}
