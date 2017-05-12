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


package euler; 

import java.io.*;
import jgfutil.*; 

public class JGFEulerBench extends Tunnel implements JGFSection3 {


  public void JGFsetsize(int size){
    this.size = size;
  }

  public void JGFinitialise(){

    JGFInstrumentor.startTimer("Section3:Euler:Init"); 

    try {
      initialise();
    }
    catch (FileNotFoundException e){
      System.err.println("Could not find file tunnel.dat"); 
      System.exit(0);
    }
    catch (IOException e) {
      System.err.println("IOException in initialisation"); 
      System.exit(0);
    }

    JGFInstrumentor.stopTimer("Section3:Euler:Init"); 

  }

  public void JGFapplication(){ 

    JGFInstrumentor.startTimer("Section3:Euler:Run");  

    runiters();

    JGFInstrumentor.stopTimer("Section3:Euler:Run");  

  } 


  public void JGFvalidate(){
    double refval[] = {0.0033831416599344965,0.006812543658280322};
    double dev = Math.abs(error - refval[size]); 
    if (dev > 1.0e-12 ){
      System.out.println("Validation failed"); 
      System.out.println("Computed RMS pressure error = " + error);
      System.out.println("Reference value = " + refval[size]); 
    }
  }

  public void JGFtidyup(){    
    a = null;  
    deltat = null;  
    opg = null;  
    pg = null;  
    pg1 = null;  
    sxi = null;  
    seta = null;  
    tg = null;  
    tg1 = null;  
    xnode = null;  
    ynode =null;  
    d = null;  
    f = null;  
    g = null;  
    r = null;  
    ug1 = null;  
    ug = null;  

    System.gc(); 
  }


  public void JGFrun(int size){

    JGFInstrumentor.addTimer("Section3:Euler:Total", "Solutions",size);
    JGFInstrumentor.addTimer("Section3:Euler:Init", "Gridpoints",size);
    JGFInstrumentor.addTimer("Section3:Euler:Run", "Timesteps",size);

    JGFsetsize(size); 

    JGFInstrumentor.startTimer("Section3:Euler:Total");

    JGFinitialise(); 
    JGFapplication(); 
    JGFvalidate(); 
    JGFtidyup(); 

    JGFInstrumentor.stopTimer("Section3:Euler:Total");

    JGFInstrumentor.addOpsToTimer("Section3:Euler:Init", (double) (imax*jmax));
    JGFInstrumentor.addOpsToTimer("Section3:Euler:Run", (double) iter);
    JGFInstrumentor.addOpsToTimer("Section3:Euler:Total", 1);

    JGFInstrumentor.printTimer("Section3:Euler:Init"); 
    JGFInstrumentor.printTimer("Section3:Euler:Run"); 
    JGFInstrumentor.printTimer("Section3:Euler:Total"); 
  }


}
 
