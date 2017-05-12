
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


import jgfutil.*; 

public class JGFAll {

  public static void main (String argv[]){

    JGFInstrumentor.printHeader(1,0);

    JGFArithBench ab = new JGFArithBench(); 
    ab.JGFrun();
    
    JGFAssignBench asb = new JGFAssignBench(); 
    asb.JGFrun();
 
    JGFCastBench cb = new JGFCastBench(); 
    cb.JGFrun();

    JGFCreateBench crb = new JGFCreateBench(); 
    crb.JGFrun();

    JGFExceptionBench eb = new JGFExceptionBench();
    eb.JGFrun();

    JGFLoopBench lb = new JGFLoopBench(); 
    lb.JGFrun();

    JGFMathBench mb = new JGFMathBench(); 
    mb.JGFrun();

    JGFMethodBench meb = new JGFMethodBench(); 
    meb.JGFrun();

    JGFSerialBench szb = new JGFSerialBench();
    szb.JGFrun();

  }

}
