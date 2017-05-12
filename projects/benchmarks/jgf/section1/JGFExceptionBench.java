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

public class JGFExceptionBench implements JGFSection1 {

  private static final int INITSIZE = 10000;
  private static final int MAXSIZE = 1000000000;
  private static final double TARGETTIME = 10.0;

  public void JGFrun(){

    int i,size;
    double time;


    JGFInstrumentor.addTimer("Section1:Exception:Throw", "exceptions");
    time = 0.0;
    size = INITSIZE;
    Exception except = new Exception();

    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Exception:Throw");
      JGFInstrumentor.startTimer("Section1:Exception:Throw");
      for (i=0; i<size; i++){

      try { throw except; } catch (Exception ecatch) {};
      try { throw except; } catch (Exception ecatch) {};
      try { throw except; } catch (Exception ecatch) {};
      try { throw except; } catch (Exception ecatch) {};
      try { throw except; } catch (Exception ecatch) {};
      try { throw except; } catch (Exception ecatch) {};
      try { throw except; } catch (Exception ecatch) {};
      try { throw except; } catch (Exception ecatch) {};

      }
      JGFInstrumentor.stopTimer("Section1:Exception:Throw");
      time = JGFInstrumentor.readTimer("Section1:Exception:Throw");
      JGFInstrumentor.addOpsToTimer("Section1:Exception:Throw", (double) 8*size);

      size *=2;

    }

    JGFInstrumentor.printperfTimer("Section1:Exception:Throw");

    JGFInstrumentor.addTimer("Section1:Exception:New", "exceptions");


    time = 0.0;
    size = INITSIZE;
 
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Exception:New");
      JGFInstrumentor.startTimer("Section1:Exception:New");
      for (i=0; i<size; i++){

      try { throw new Exception(); } catch (Exception ecatch) {};
      try { throw new Exception(); } catch (Exception ecatch) {};
      try { throw new Exception(); } catch (Exception ecatch) {};
      try { throw new Exception(); } catch (Exception ecatch) {};
      try { throw new Exception(); } catch (Exception ecatch) {};
      try { throw new Exception(); } catch (Exception ecatch) {};
      try { throw new Exception(); } catch (Exception ecatch) {};
      try { throw new Exception(); } catch (Exception ecatch) {};

      }
      JGFInstrumentor.stopTimer("Section1:Exception:New");
      time = JGFInstrumentor.readTimer("Section1:Exception:New");
      JGFInstrumentor.addOpsToTimer("Section1:Exception:New", (double) 8*size);

      size *=2;

    }

    JGFInstrumentor.printperfTimer("Section1:Exception:New");

    JGFInstrumentor.addTimer("Section1:Exception:Method", "exceptions");


    time = 0.0;
    size = INITSIZE;

    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Exception:Method");
      JGFInstrumentor.startTimer("Section1:Exception:Method");
      for (i=0; i<size; i++){

      try { exceptthrow(); } catch (Exception ecatch) {};
      try { exceptthrow(); } catch (Exception ecatch) {};
      try { exceptthrow(); } catch (Exception ecatch) {};
      try { exceptthrow(); } catch (Exception ecatch) {};
      try { exceptthrow(); } catch (Exception ecatch) {};
      try { exceptthrow(); } catch (Exception ecatch) {};
      try { exceptthrow(); } catch (Exception ecatch) {};
      try { exceptthrow(); } catch (Exception ecatch) {};

      }
      JGFInstrumentor.stopTimer("Section1:Exception:Method");
      time = JGFInstrumentor.readTimer("Section1:Exception:Method");
      JGFInstrumentor.addOpsToTimer("Section1:Exception:Method", (double) 8*size);

      size *=2;

    }

    JGFInstrumentor.printperfTimer("Section1:Exception:Method");


  }

  public static void main(String[] argv) {

   JGFExceptionBench eb = new JGFExceptionBench();
   eb.JGFrun();

  }

    public static void exceptthrow() throws Exception {

    Exception except = new Exception();
    throw except;
  }

}
