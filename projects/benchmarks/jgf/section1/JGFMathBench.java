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

public class JGFMathBench implements JGFSection1{


  private static final int INITSIZE = 10000;
  private static final int MAXSIZE = 1000000000;
  private static final double TARGETTIME = 10.0; 
  

  public void JGFrun(){

    int i,size;
    double time; 
    int i1,i2,i3,i4; 
    long l1,l2,l3,l4;
    float f1,f2,f3,f4;
    double d1,d2,d3,d4;
  
    JGFInstrumentor.addTimer("Section1:Math:AbsInt", "operations");

    time = 0.0; 
    size = INITSIZE;
    i1=1;i2=2;i3=3;i4=4; 
    
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Math:AbsInt"); 
      JGFInstrumentor.startTimer("Section1:Math:AbsInt"); 
      for (i=0; i<size; i++){
          i1=Math.abs(i2); i2=Math.abs(i3); i3=Math.abs(i4); i4=Math.abs(i1);
          i1=Math.abs(i2); i2=Math.abs(i3); i3=Math.abs(i4); i4=Math.abs(i1);
          i1=Math.abs(i2); i2=Math.abs(i3); i3=Math.abs(i4); i4=Math.abs(i1);
          i1=Math.abs(i2); i2=Math.abs(i3); i3=Math.abs(i4); i4=Math.abs(i1);
        }
      JGFInstrumentor.stopTimer("Section1:Math:AbsInt"); 

      // try to defeat dead code elimination 
      if (i1 == -1) System.out.println(i1); 
      time = JGFInstrumentor.readTimer("Section1:Math:AbsInt"); 
      JGFInstrumentor.addOpsToTimer("Section1:Math:AbsInt", (double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Math:AbsInt"); 


    JGFInstrumentor.addTimer("Section1:Math:AbsLong", "operations");

    time = 0.0; 
    size = INITSIZE;
    l1=1;l2=2;l3=3;l4=4; 
    
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Math:AbsLong"); 
      JGFInstrumentor.startTimer("Section1:Math:AbsLong"); 
      for (i=0; i<size; i++){
          l1=Math.abs(l2); l2=Math.abs(l3); l3=Math.abs(l4); l4=Math.abs(l1);
          l1=Math.abs(l2); l2=Math.abs(l3); l3=Math.abs(l4); l4=Math.abs(l1);
          l1=Math.abs(l2); l2=Math.abs(l3); l3=Math.abs(l4); l4=Math.abs(l1);
          l1=Math.abs(l2); l2=Math.abs(l3); l3=Math.abs(l4); l4=Math.abs(l1);
        }
      JGFInstrumentor.stopTimer("Section1:Math:AbsLong"); 

      // try to defeat dead code elimination 
      if (l1 == -1) System.out.println(l1); 
      time = JGFInstrumentor.readTimer("Section1:Math:AbsLong"); 
      JGFInstrumentor.addOpsToTimer("Section1:Math:AbsLong", (double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Math:AbsLong"); 

    JGFInstrumentor.addTimer("Section1:Math:AbsFloat", "operations");

    time = 0.0; 
    size = INITSIZE;
    f1=1.0F;f2=2.0F;f3=3.0F;f4=4.0F; 
    
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Math:AbsFloat"); 
      JGFInstrumentor.startTimer("Section1:Math:AbsFloat"); 
      for (i=0; i<size; i++){
          f1=Math.abs(f2); f2=Math.abs(f3); f3=Math.abs(f4); f4=Math.abs(f1);
          f1=Math.abs(f2); f2=Math.abs(f3); f3=Math.abs(f4); f4=Math.abs(f1);
          f1=Math.abs(f2); f2=Math.abs(f3); f3=Math.abs(f4); f4=Math.abs(f1);
          f1=Math.abs(f2); f2=Math.abs(f3); f3=Math.abs(f4); f4=Math.abs(f1);
        }
      JGFInstrumentor.stopTimer("Section1:Math:AbsFloat"); 

      // try to defeat dead code elimination 
      if (f1 == -1.0F) System.out.println(f1); 
      time = JGFInstrumentor.readTimer("Section1:Math:AbsFloat"); 
      JGFInstrumentor.addOpsToTimer("Section1:Math:AbsFloat", (double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Math:AbsFloat"); 


    JGFInstrumentor.addTimer("Section1:Math:AbsDouble", "operations");

    time = 0.0; 
    size = INITSIZE;
    d1=1.0D;d2=2.0D;d3=3.0D;d4=4.0D; 
    
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Math:AbsDouble"); 
      JGFInstrumentor.startTimer("Section1:Math:AbsDouble"); 
      for (i=0; i<size; i++){
          d1=Math.abs(d2); d2=Math.abs(d3); d3=Math.abs(d4); d4=Math.abs(d1);
          d1=Math.abs(d2); d2=Math.abs(d3); d3=Math.abs(d4); d4=Math.abs(d1);
          d1=Math.abs(d2); d2=Math.abs(d3); d3=Math.abs(d4); d4=Math.abs(d1);
          d1=Math.abs(d2); d2=Math.abs(d3); d3=Math.abs(d4); d4=Math.abs(d1);
        }
      JGFInstrumentor.stopTimer("Section1:Math:AbsDouble"); 

      // try to defeat dead code elimination 
      if (d1 == -1.0D) System.out.println(d1); 
      time = JGFInstrumentor.readTimer("Section1:Math:AbsDouble"); 
      JGFInstrumentor.addOpsToTimer("Section1:Math:AbsDouble", (double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Math:AbsDouble"); 

    JGFInstrumentor.addTimer("Section1:Math:MaxInt", "operations");

    time = 0.0; 
    size = INITSIZE;
    i1=1;i2=2;i3=3;i4=4; 
    
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Math:MaxInt"); 
      JGFInstrumentor.startTimer("Section1:Math:MaxInt"); 
      for (i=0; i<size; i++){
          i1=Math.max(i2,i3); i2=Math.max(i3,i4); i3=Math.max(i4,i1); i4=Math.max(i1,i2);
          i1=Math.max(i2,i3); i2=Math.max(i3,i4); i3=Math.max(i4,i1); i4=Math.max(i1,i2);
          i1=Math.max(i2,i3); i2=Math.max(i3,i4); i3=Math.max(i4,i1); i4=Math.max(i1,i2);
          i1=Math.max(i2,i3); i2=Math.max(i3,i4); i3=Math.max(i4,i1); i4=Math.max(i1,i2);
        }
      JGFInstrumentor.stopTimer("Section1:Math:MaxInt"); 

      // try to defeat dead code elimination 
      if (i1 == -1) System.out.println(i1); 
      time = JGFInstrumentor.readTimer("Section1:Math:MaxInt"); 
      JGFInstrumentor.addOpsToTimer("Section1:Math:MaxInt", (double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Math:MaxInt"); 


    JGFInstrumentor.addTimer("Section1:Math:MaxLong", "operations");

    time = 0.0; 
    size = INITSIZE;
    l1=1;l2=2;l3=3;l4=4; 
    
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Math:MaxLong"); 
      JGFInstrumentor.startTimer("Section1:Math:MaxLong"); 
      for (i=0; i<size; i++){
          l1=Math.max(l2,l3); l2=Math.max(l3,l4); l3=Math.max(l4,l1); l4=Math.max(l1,l2);
          l1=Math.max(l2,l3); l2=Math.max(l3,l4); l3=Math.max(l4,l1); l4=Math.max(l1,l2);
          l1=Math.max(l2,l3); l2=Math.max(l3,l4); l3=Math.max(l4,l1); l4=Math.max(l1,l2);
          l1=Math.max(l2,l3); l2=Math.max(l3,l4); l3=Math.max(l4,l1); l4=Math.max(l1,l2);
        }
      JGFInstrumentor.stopTimer("Section1:Math:MaxLong"); 

      // try to defeat dead code elimination 
      if (l1 == -1) System.out.println(l1); 
      time = JGFInstrumentor.readTimer("Section1:Math:MaxLong"); 
      JGFInstrumentor.addOpsToTimer("Section1:Math:MaxLong", (double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Math:MaxLong"); 

    JGFInstrumentor.addTimer("Section1:Math:MaxFloat", "operations");

    time = 0.0; 
    size = INITSIZE;
    f1=1.0F;f2=2.0F;f3=3.0F;f4=4.0F; 
    
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Math:MaxFloat"); 
      JGFInstrumentor.startTimer("Section1:Math:MaxFloat"); 
      for (i=0; i<size; i++){
          f1=Math.max(f2,f3); f2=Math.max(f3,f4); f3=Math.max(f4,f1); f4=Math.max(f1,f2);
          f1=Math.max(f2,f3); f2=Math.max(f3,f4); f3=Math.max(f4,f1); f4=Math.max(f1,f2);
          f1=Math.max(f2,f3); f2=Math.max(f3,f4); f3=Math.max(f4,f1); f4=Math.max(f1,f2);
          f1=Math.max(f2,f3); f2=Math.max(f3,f4); f3=Math.max(f4,f1); f4=Math.max(f1,f2);
        }
      JGFInstrumentor.stopTimer("Section1:Math:MaxFloat"); 

      // try to defeat dead code elimination 
      if (f1 == -1.0F) System.out.println(f1); 
      time = JGFInstrumentor.readTimer("Section1:Math:MaxFloat"); 
      JGFInstrumentor.addOpsToTimer("Section1:Math:MaxFloat", (double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Math:MaxFloat"); 


    JGFInstrumentor.addTimer("Section1:Math:MaxDouble", "operations");

    time = 0.0; 
    size = INITSIZE;
    d1=1.0D;d2=2.0D;d3=3.0D;d4=4.0D; 
    
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Math:MaxDouble"); 
      JGFInstrumentor.startTimer("Section1:Math:MaxDouble"); 
      for (i=0; i<size; i++){
          d1=Math.max(d2,d3); d2=Math.max(d3,d4); d3=Math.max(d4,d1); d4=Math.max(d1,d2);
          d1=Math.max(d2,d3); d2=Math.max(d3,d4); d3=Math.max(d4,d1); d4=Math.max(d1,d2);
          d1=Math.max(d2,d3); d2=Math.max(d3,d4); d3=Math.max(d4,d1); d4=Math.max(d1,d2);
          d1=Math.max(d2,d3); d2=Math.max(d3,d4); d3=Math.max(d4,d1); d4=Math.max(d1,d2);
        }
      JGFInstrumentor.stopTimer("Section1:Math:MaxDouble"); 

      // try to defeat dead code elimination 
      if (d1 == -1.0D) System.out.println(d1); 
      time = JGFInstrumentor.readTimer("Section1:Math:MaxDouble"); 
      JGFInstrumentor.addOpsToTimer("Section1:Math:MaxDouble", (double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Math:MaxDouble"); 

    JGFInstrumentor.addTimer("Section1:Math:MinInt", "operations");

    time = 0.0; 
    size = INITSIZE;
    i1=1;i2=2;i3=3;i4=4; 
    
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Math:MinInt"); 
      JGFInstrumentor.startTimer("Section1:Math:MinInt"); 
      for (i=0; i<size; i++){
          i1=Math.min(i2,i3); i2=Math.min(i3,i4); i3=Math.min(i4,i1); i4=Math.min(i1,i2);
          i1=Math.min(i2,i3); i2=Math.min(i3,i4); i3=Math.min(i4,i1); i4=Math.min(i1,i2);
          i1=Math.min(i2,i3); i2=Math.min(i3,i4); i3=Math.min(i4,i1); i4=Math.min(i1,i2);
          i1=Math.min(i2,i3); i2=Math.min(i3,i4); i3=Math.min(i4,i1); i4=Math.min(i1,i2);
        }
      JGFInstrumentor.stopTimer("Section1:Math:MinInt"); 

      // try to defeat dead code elimination 
      if (i1 == -1) System.out.println(i1); 
      time = JGFInstrumentor.readTimer("Section1:Math:MinInt"); 
      JGFInstrumentor.addOpsToTimer("Section1:Math:MinInt", (double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Math:MinInt"); 


    JGFInstrumentor.addTimer("Section1:Math:MinLong", "operations");

    time = 0.0; 
    size = INITSIZE;
    l1=1;l2=2;l3=3;l4=4; 
    
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Math:MinLong"); 
      JGFInstrumentor.startTimer("Section1:Math:MinLong"); 
      for (i=0; i<size; i++){
          l1=Math.min(l2,l3); l2=Math.min(l3,l4); l3=Math.min(l4,l1); l4=Math.min(l1,l2);
          l1=Math.min(l2,l3); l2=Math.min(l3,l4); l3=Math.min(l4,l1); l4=Math.min(l1,l2);
          l1=Math.min(l2,l3); l2=Math.min(l3,l4); l3=Math.min(l4,l1); l4=Math.min(l1,l2);
          l1=Math.min(l2,l3); l2=Math.min(l3,l4); l3=Math.min(l4,l1); l4=Math.min(l1,l2);
        }
      JGFInstrumentor.stopTimer("Section1:Math:MinLong"); 

      // try to defeat dead code elimination 
      if (l1 == -1) System.out.println(l1); 
      time = JGFInstrumentor.readTimer("Section1:Math:MinLong"); 
      JGFInstrumentor.addOpsToTimer("Section1:Math:MinLong", (double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Math:MinLong"); 

    JGFInstrumentor.addTimer("Section1:Math:MinFloat", "operations");

    time = 0.0; 
    size = INITSIZE;
    f1=1.0F;f2=2.0F;f3=3.0F;f4=4.0F; 
    
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Math:MinFloat"); 
      JGFInstrumentor.startTimer("Section1:Math:MinFloat"); 
      for (i=0; i<size; i++){
          f1=Math.min(f2,f3); f2=Math.min(f3,f4); f3=Math.min(f4,f1); f4=Math.min(f1,f2);
          f1=Math.min(f2,f3); f2=Math.min(f3,f4); f3=Math.min(f4,f1); f4=Math.min(f1,f2);
          f1=Math.min(f2,f3); f2=Math.min(f3,f4); f3=Math.min(f4,f1); f4=Math.min(f1,f2);
          f1=Math.min(f2,f3); f2=Math.min(f3,f4); f3=Math.min(f4,f1); f4=Math.min(f1,f2);
        }
      JGFInstrumentor.stopTimer("Section1:Math:MinFloat"); 

      // try to defeat dead code elimination 
      if (f1 == -1.0F) System.out.println(f1); 
      time = JGFInstrumentor.readTimer("Section1:Math:MinFloat"); 
      JGFInstrumentor.addOpsToTimer("Section1:Math:MinFloat", (double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Math:MinFloat"); 

    JGFInstrumentor.addTimer("Section1:Math:MinDouble", "operations");

    time = 0.0; 
    size = INITSIZE;
    d1=1.0D;d2=2.0D;d3=3.0D;d4=4.0D; 
    
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Math:MinDouble"); 
      JGFInstrumentor.startTimer("Section1:Math:MinDouble"); 
      for (i=0; i<size; i++){
          d1=Math.min(d2,d3); d2=Math.min(d3,d4); d3=Math.min(d4,d1); d4=Math.min(d1,d2);
          d1=Math.min(d2,d3); d2=Math.min(d3,d4); d3=Math.min(d4,d1); d4=Math.min(d1,d2);
          d1=Math.min(d2,d3); d2=Math.min(d3,d4); d3=Math.min(d4,d1); d4=Math.min(d1,d2);
          d1=Math.min(d2,d3); d2=Math.min(d3,d4); d3=Math.min(d4,d1); d4=Math.min(d1,d2);
        }
      JGFInstrumentor.stopTimer("Section1:Math:MinDouble"); 

      // try to defeat dead code elimination 
      if (d1 == -1.0D) System.out.println(d1); 
      time = JGFInstrumentor.readTimer("Section1:Math:MinDouble"); 
      JGFInstrumentor.addOpsToTimer("Section1:Math:MinDouble", (double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Math:MinDouble"); 


    JGFInstrumentor.addTimer("Section1:Math:SinDouble", "operations");

    time = 0.0; 
    size = INITSIZE;
    d1=1.0D;d2=2.0D;d3=3.0D;d4=4.0D; 
    
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Math:SinDouble"); 
      JGFInstrumentor.startTimer("Section1:Math:SinDouble"); 
      for (i=0; i<size; i++){
          d1=Math.sin(d2); d2=Math.sin(d3); d3=Math.sin(d4); d4=Math.sin(d1);
          d1=Math.sin(d2); d2=Math.sin(d3); d3=Math.sin(d4); d4=Math.sin(d1);
          d1=Math.sin(d2); d2=Math.sin(d3); d3=Math.sin(d4); d4=Math.sin(d1);
          d1=Math.sin(d2); d2=Math.sin(d3); d3=Math.sin(d4); d4=Math.sin(d1);
        }
      JGFInstrumentor.stopTimer("Section1:Math:SinDouble"); 

      // try to defeat dead code elimination 
      if (d1 == -1.0D) System.out.println(d1); 
      time = JGFInstrumentor.readTimer("Section1:Math:SinDouble"); 
      JGFInstrumentor.addOpsToTimer("Section1:Math:SinDouble", (double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Math:SinDouble"); 

    JGFInstrumentor.addTimer("Section1:Math:CosDouble", "operations");

    time = 0.0; 
    size = INITSIZE;
    d1=1.0D;d2=2.0D;d3=3.0D;d4=4.0D; 
    
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Math:CosDouble"); 
      JGFInstrumentor.startTimer("Section1:Math:CosDouble"); 
      for (i=0; i<size; i++){
          d1=Math.cos(d2); d2=Math.cos(d3); d3=Math.cos(d4); d4=Math.cos(d1);
          d1=Math.cos(d2); d2=Math.cos(d3); d3=Math.cos(d4); d4=Math.cos(d1);
          d1=Math.cos(d2); d2=Math.cos(d3); d3=Math.cos(d4); d4=Math.cos(d1);
          d1=Math.cos(d2); d2=Math.cos(d3); d3=Math.cos(d4); d4=Math.cos(d1);
        }
      JGFInstrumentor.stopTimer("Section1:Math:CosDouble"); 

      // try to defeat dead code elimination 
      if (d1 == -1.0D) System.out.println(d1); 
      time = JGFInstrumentor.readTimer("Section1:Math:CosDouble"); 
      JGFInstrumentor.addOpsToTimer("Section1:Math:CosDouble", (double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Math:CosDouble"); 

    JGFInstrumentor.addTimer("Section1:Math:TanDouble", "operations");

    time = 0.0; 
    size = INITSIZE;
    d1=1.0D;d2=2.0D;d3=3.0D;d4=4.0D; 
    
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Math:TanDouble"); 
      JGFInstrumentor.startTimer("Section1:Math:TanDouble"); 
      for (i=0; i<size; i++){
          d1=Math.tan(d2); d2=Math.tan(d3); d3=Math.tan(d4); d4=Math.tan(d1);
          d1=Math.tan(d2); d2=Math.tan(d3); d3=Math.tan(d4); d4=Math.tan(d1);
          d1=Math.tan(d2); d2=Math.tan(d3); d3=Math.tan(d4); d4=Math.tan(d1);
          d1=Math.tan(d2); d2=Math.tan(d3); d3=Math.tan(d4); d4=Math.tan(d1);
        }
      JGFInstrumentor.stopTimer("Section1:Math:TanDouble"); 

      // try to defeat dead code elimination 
      if (d1 == -1.0D) System.out.println(d1); 
      time = JGFInstrumentor.readTimer("Section1:Math:TanDouble"); 
      JGFInstrumentor.addOpsToTimer("Section1:Math:TanDouble", (double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Math:TanDouble"); 

    JGFInstrumentor.addTimer("Section1:Math:AsinDouble", "operations");

    time = 0.0; 
    size = INITSIZE;
    d1=1.0D;d2=1.0D;d3=3.0D;d4=4.0D; 
    
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Math:AsinDouble"); 
      JGFInstrumentor.startTimer("Section1:Math:AsinDouble"); 
      for (i=0; i<size; i++){
          d1=Math.asin(d2*0.5D); d2=Math.asin(d3*0.5D); d3=Math.asin(d4*0.5D); d4=Math.asin(d1*0.5D);
          d1=Math.asin(d2*0.5D); d2=Math.asin(d3*0.5D); d3=Math.asin(d4*0.5D); d4=Math.asin(d1*0.5D);
          d1=Math.asin(d2*0.5D); d2=Math.asin(d3*0.5D); d3=Math.asin(d4*0.5D); d4=Math.asin(d1*0.5D);
          d1=Math.asin(d2*0.5D); d2=Math.asin(d3*0.5D); d3=Math.asin(d4*0.5D); d4=Math.asin(d1*0.5D);
        }
      JGFInstrumentor.stopTimer("Section1:Math:AsinDouble"); 

      // try to defeat dead code elimination 
      if (d1 == -1.0D) System.out.println(d1); 
      time = JGFInstrumentor.readTimer("Section1:Math:AsinDouble"); 
      JGFInstrumentor.addOpsToTimer("Section1:Math:AsinDouble", (double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Math:AsinDouble"); 

    JGFInstrumentor.addTimer("Section1:Math:AcosDouble", "operations");

    time = 0.0; 
    size = INITSIZE;
    d1=1.0D;d2=1.0D;d3=3.0D;d4=4.0D; 
    
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Math:AcosDouble"); 
      JGFInstrumentor.startTimer("Section1:Math:AcosDouble"); 
      for (i=0; i<size; i++){
          d1=Math.acos(d2*0.5D); d2=Math.acos(d3*0.5D); d3=Math.acos(d4*0.5D); d4=Math.acos(d1*0.5D);
          d1=Math.acos(d2*0.5D); d2=Math.acos(d3*0.5D); d3=Math.acos(d4*0.5D); d4=Math.acos(d1*0.5D);
          d1=Math.acos(d2*0.5D); d2=Math.acos(d3*0.5D); d3=Math.acos(d4*0.5D); d4=Math.acos(d1*0.5D);
          d1=Math.acos(d2*0.5D); d2=Math.acos(d3*0.5D); d3=Math.acos(d4*0.5D); d4=Math.acos(d1*0.5D);
        }
      JGFInstrumentor.stopTimer("Section1:Math:AcosDouble"); 

      // try to defeat dead code elimination 
      if (d1 == -1.0D) System.out.println(d1); 
      time = JGFInstrumentor.readTimer("Section1:Math:AcosDouble"); 
      JGFInstrumentor.addOpsToTimer("Section1:Math:AcosDouble", (double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Math:AcosDouble"); 

    JGFInstrumentor.addTimer("Section1:Math:AtanDouble", "operations");

    time = 0.0; 
    size = INITSIZE;
    d1=1.0D;d2=1.0D;d3=3.0D;d4=4.0D; 
    
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Math:AtanDouble"); 
      JGFInstrumentor.startTimer("Section1:Math:AtanDouble"); 
      for (i=0; i<size; i++){
          d1=Math.atan(d2*0.5D); d2=Math.atan(d3*0.5D); d3=Math.atan(d4*0.5D); d4=Math.atan(d1*0.5D);
          d1=Math.atan(d2*0.5D); d2=Math.atan(d3*0.5D); d3=Math.atan(d4*0.5D); d4=Math.atan(d1*0.5D);
          d1=Math.atan(d2*0.5D); d2=Math.atan(d3*0.5D); d3=Math.atan(d4*0.5D); d4=Math.atan(d1*0.5D);
          d1=Math.atan(d2*0.5D); d2=Math.atan(d3*0.5D); d3=Math.atan(d4*0.5D); d4=Math.atan(d1*0.5D);
        }
      JGFInstrumentor.stopTimer("Section1:Math:AtanDouble"); 

      // try to defeat dead code elimination 
      if (d1 == -1.0D) System.out.println(d1); 
      time = JGFInstrumentor.readTimer("Section1:Math:AtanDouble"); 
      JGFInstrumentor.addOpsToTimer("Section1:Math:AtanDouble", (double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Math:AtanDouble"); 

    JGFInstrumentor.addTimer("Section1:Math:Atan2Double", "operations");

    time = 0.0; 
    size = INITSIZE;
    d1=1.0D;d2=1.0D;d3=3.0D;d4=4.0D; 
    
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Math:Atan2Double"); 
      JGFInstrumentor.startTimer("Section1:Math:Atan2Double"); 
      for (i=0; i<size; i++){
          d1=Math.atan2(d2,2.0D); d2=Math.atan2(d3,2.0D); d3=Math.atan2(d4,2.0D); d4=Math.atan2(d1,2.0D);
          d1=Math.atan2(d2,2.0D); d2=Math.atan2(d3,2.0D); d3=Math.atan2(d4,2.0D); d4=Math.atan2(d1,2.0D);
          d1=Math.atan2(d2,2.0D); d2=Math.atan2(d3,2.0D); d3=Math.atan2(d4,2.0D); d4=Math.atan2(d1,2.0D);
          d1=Math.atan2(d2,2.0D); d2=Math.atan2(d3,2.0D); d3=Math.atan2(d4,2.0D); d4=Math.atan2(d1,2.0D);
        }
      JGFInstrumentor.stopTimer("Section1:Math:Atan2Double"); 

      // try to defeat dead code elimination 
      if (d1 == -1.0D) System.out.println(d1); 
      time = JGFInstrumentor.readTimer("Section1:Math:Atan2Double"); 
      JGFInstrumentor.addOpsToTimer("Section1:Math:Atan2Double", (double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Math:Atan2Double"); 


    JGFInstrumentor.addTimer("Section1:Math:FloorDouble", "operations");

    time = 0.0; 
    size = INITSIZE;
    d1=1.0D;d2=2.0D;d3=3.0D;d4=4.0D; 
    
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Math:FloorDouble"); 
      JGFInstrumentor.startTimer("Section1:Math:FloorDouble"); 
      for (i=0; i<size; i++){
          d1=Math.floor(d2); d2=Math.floor(d3); d3=Math.floor(d4); d4=Math.floor(d1);
          d1=Math.floor(d2); d2=Math.floor(d3); d3=Math.floor(d4); d4=Math.floor(d1);
          d1=Math.floor(d2); d2=Math.floor(d3); d3=Math.floor(d4); d4=Math.floor(d1);
          d1=Math.floor(d2); d2=Math.floor(d3); d3=Math.floor(d4); d4=Math.floor(d1);
        }
      JGFInstrumentor.stopTimer("Section1:Math:FloorDouble"); 

      // try to defeat dead code elimination 
      if (d1 == -1.0D) System.out.println(d1); 
      time = JGFInstrumentor.readTimer("Section1:Math:FloorDouble"); 
      JGFInstrumentor.addOpsToTimer("Section1:Math:FloorDouble", (double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Math:FloorDouble"); 

    JGFInstrumentor.addTimer("Section1:Math:CeilDouble", "operations");

    time = 0.0; 
    size = INITSIZE;
    d1=1.0D;d2=2.0D;d3=3.0D;d4=4.0D; 
    
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Math:CeilDouble"); 
      JGFInstrumentor.startTimer("Section1:Math:CeilDouble"); 
      for (i=0; i<size; i++){
          d1=Math.ceil(d2); d2=Math.ceil(d3); d3=Math.ceil(d4); d4=Math.ceil(d1);
          d1=Math.ceil(d2); d2=Math.ceil(d3); d3=Math.ceil(d4); d4=Math.ceil(d1);
          d1=Math.ceil(d2); d2=Math.ceil(d3); d3=Math.ceil(d4); d4=Math.ceil(d1);
          d1=Math.ceil(d2); d2=Math.ceil(d3); d3=Math.ceil(d4); d4=Math.ceil(d1);
        }
      JGFInstrumentor.stopTimer("Section1:Math:CeilDouble"); 

      // try to defeat dead code elimination 
      if (d1 == -1.0D) System.out.println(d1); 
      time = JGFInstrumentor.readTimer("Section1:Math:CeilDouble"); 
      JGFInstrumentor.addOpsToTimer("Section1:Math:CeilDouble", (double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Math:CeilDouble"); 

    JGFInstrumentor.addTimer("Section1:Math:SqrtDouble", "operations");

    time = 0.0; 
    size = INITSIZE;
    d1=1.0D;d2=2.0D;d3=3.0D;d4=4.0D; 
    
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Math:SqrtDouble"); 
      JGFInstrumentor.startTimer("Section1:Math:SqrtDouble"); 
      for (i=0; i<size; i++){
          d1=Math.sqrt(d2); d2=Math.sqrt(d3); d3=Math.sqrt(d4); d4=Math.sqrt(d1);
          d1=Math.sqrt(d2); d2=Math.sqrt(d3); d3=Math.sqrt(d4); d4=Math.sqrt(d1);
          d1=Math.sqrt(d2); d2=Math.sqrt(d3); d3=Math.sqrt(d4); d4=Math.sqrt(d1);
          d1=Math.sqrt(d2); d2=Math.sqrt(d3); d3=Math.sqrt(d4); d4=Math.sqrt(d1);
        }
      JGFInstrumentor.stopTimer("Section1:Math:SqrtDouble"); 

      // try to defeat dead code elimination 
      if (d1 == -1.0D) System.out.println(d1); 
      time = JGFInstrumentor.readTimer("Section1:Math:SqrtDouble"); 
      JGFInstrumentor.addOpsToTimer("Section1:Math:SqrtDouble", (double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Math:SqrtDouble"); 

    JGFInstrumentor.addTimer("Section1:Math:ExpDouble", "operations");

    time = 0.0; 
    size = INITSIZE;
    d1=1.0D;d2=-2.0D;d3=3.0D;d4=4.0D; 
    
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Math:ExpDouble"); 
      JGFInstrumentor.startTimer("Section1:Math:ExpDouble"); 
      for (i=0; i<size; i++){
          d1=Math.exp(d2-2.0D); d2=Math.exp(d3-2.0D); d3=Math.exp(d4-2.0D); d4=Math.exp(d1-2.0D);
          d1=Math.exp(d2-2.0D); d2=Math.exp(d3-2.0D); d3=Math.exp(d4-2.0D); d4=Math.exp(d1-2.0D);
          d1=Math.exp(d2-2.0D); d2=Math.exp(d3-2.0D); d3=Math.exp(d4-2.0D); d4=Math.exp(d1-2.0D);
          d1=Math.exp(d2-2.0D); d2=Math.exp(d3-2.0D); d3=Math.exp(d4-2.0D); d4=Math.exp(d1-2.0D);
        }
      JGFInstrumentor.stopTimer("Section1:Math:ExpDouble"); 

      // try to defeat dead code elimination 
      if (d1 == -1.0D) System.out.println(d1); 
      time = JGFInstrumentor.readTimer("Section1:Math:ExpDouble"); 
      JGFInstrumentor.addOpsToTimer("Section1:Math:ExpDouble", (double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Math:ExpDouble"); 

    JGFInstrumentor.addTimer("Section1:Math:LogDouble", "operations");

    time = 0.0; 
    size = INITSIZE;
    d1=1.0D;d2=2.0D;d3=3.0D;d4=4.0D; 
    
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Math:LogDouble"); 
      JGFInstrumentor.startTimer("Section1:Math:LogDouble"); 
      for (i=0; i<size; i++){
          d1=Math.log(d2+2.0D); d2=Math.log(d3+2.0D); d3=Math.log(d4+2.0D); d4=Math.log(d1+2.0D);
          d1=Math.log(d2+2.0D); d2=Math.log(d3+2.0D); d3=Math.log(d4+2.0D); d4=Math.log(d1+2.0D);
          d1=Math.log(d2+2.0D); d2=Math.log(d3+2.0D); d3=Math.log(d4+2.0D); d4=Math.log(d1+2.0D);
          d1=Math.log(d2+2.0D); d2=Math.log(d3+2.0D); d3=Math.log(d4+2.0D); d4=Math.log(d1+2.0D);
        }
      JGFInstrumentor.stopTimer("Section1:Math:LogDouble"); 

      // try to defeat dead code elimination 
      if (d1 == -1.0D) System.out.println(d1); 
      time = JGFInstrumentor.readTimer("Section1:Math:LogDouble"); 
      JGFInstrumentor.addOpsToTimer("Section1:Math:LogDouble", (double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Math:LogDouble"); 
   
    JGFInstrumentor.addTimer("Section1:Math:PowDouble", "operations");

    time = 0.0; 
    size = INITSIZE;
    d1=0.1D;d2=0.2D;d3=0.3D;d4=0.4D; 
    
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Math:PowDouble"); 
      JGFInstrumentor.startTimer("Section1:Math:PowDouble"); 
      for (i=0; i<size; i++){
          d1=Math.pow(d2,d3); d2=Math.pow(d3,d4); d3=Math.pow(d4,d1); d4=Math.pow(d1,d2);
          d1=Math.pow(d2,d3); d2=Math.pow(d3,d4); d3=Math.pow(d4,d1); d4=Math.pow(d1,d2);
          d1=Math.pow(d2,d3); d2=Math.pow(d3,d4); d3=Math.pow(d4,d1); d4=Math.pow(d1,d2);
          d1=Math.pow(d2,d3); d2=Math.pow(d3,d4); d3=Math.pow(d4,d1); d4=Math.pow(d1,d2);
        }
      JGFInstrumentor.stopTimer("Section1:Math:PowDouble"); 

      // try to defeat dead code elimination 
      if (d1 == -1.0D) System.out.println(d1); 
      time = JGFInstrumentor.readTimer("Section1:Math:PowDouble"); 
      JGFInstrumentor.addOpsToTimer("Section1:Math:PowDouble", (double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Math:PowDouble"); 
   
    JGFInstrumentor.addTimer("Section1:Math:RintDouble", "operations");

    time = 0.0; 
    size = INITSIZE;
    d1=1.0D;d2=2.0D;d3=3.0D;d4=4.0D; 
    
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Math:RintDouble"); 
      JGFInstrumentor.startTimer("Section1:Math:RintDouble"); 
      for (i=0; i<size; i++){
          d1=Math.rint(d2); d2=Math.rint(d3); d3=Math.rint(d4); d4=Math.rint(d1);
          d1=Math.rint(d2); d2=Math.rint(d3); d3=Math.rint(d4); d4=Math.rint(d1);
          d1=Math.rint(d2); d2=Math.rint(d3); d3=Math.rint(d4); d4=Math.rint(d1);
          d1=Math.rint(d2); d2=Math.rint(d3); d3=Math.rint(d4); d4=Math.rint(d1);
        }
      JGFInstrumentor.stopTimer("Section1:Math:RintDouble"); 

      // try to defeat dead code elimination 
      if (d1 == -1.0D) System.out.println(d1); 
      time = JGFInstrumentor.readTimer("Section1:Math:RintDouble"); 
      JGFInstrumentor.addOpsToTimer("Section1:Math:RintDouble", (double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Math:RintDouble"); 

    JGFInstrumentor.addTimer("Section1:Math:Random", "operations");

    time = 0.0; 
    size = INITSIZE;
    d1=1.0D;d2=2.0D;d3=3.0D;d4=4.0D; 
    
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Math:Random"); 
      JGFInstrumentor.startTimer("Section1:Math:Random"); 
      for (i=0; i<size; i++){
          d1=Math.random(); d1=Math.random(); d1=Math.random(); d1=Math.random();
          d1=Math.random(); d1=Math.random(); d1=Math.random(); d1=Math.random();
          d1=Math.random(); d1=Math.random(); d1=Math.random(); d1=Math.random();
          d1=Math.random(); d1=Math.random(); d1=Math.random(); d1=Math.random();
        }
      JGFInstrumentor.stopTimer("Section1:Math:Random"); 

      // try to defeat dead code elimination 
      if (d1 == -1.0D) System.out.println(d1); 
      time = JGFInstrumentor.readTimer("Section1:Math:Random"); 
      JGFInstrumentor.addOpsToTimer("Section1:Math:Random", (double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Math:Random"); 

    JGFInstrumentor.addTimer("Section1:Math:RoundFloat", "operations");

    time = 0.0; 
    size = INITSIZE;
    i1=1;i2=2;i3=3;i4=4; 
    
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Math:RoundFloat"); 
      JGFInstrumentor.startTimer("Section1:Math:RoundFloat"); 
      for (i=0; i<size; i++){
          i1=Math.round((float) i2); i2=Math.round((float) i3); i1=Math.round((float) i4); i4=Math.round((float) i1);
          i1=Math.round((float) i2); i2=Math.round((float) i3); i1=Math.round((float) i4); i4=Math.round((float) i1);
          i1=Math.round((float) i2); i2=Math.round((float) i3); i1=Math.round((float) i4); i4=Math.round((float) i1);
          i1=Math.round((float) i2); i2=Math.round((float) i3); i1=Math.round((float) i4); i4=Math.round((float) i1);
        }
      JGFInstrumentor.stopTimer("Section1:Math:RoundFloat"); 

      // try to defeat dead code elimination 
      if (i1 == -1) System.out.println(i1); 
      time = JGFInstrumentor.readTimer("Section1:Math:RoundFloat"); 
      JGFInstrumentor.addOpsToTimer("Section1:Math:RoundFloat", (double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Math:RoundFloat"); 

    JGFInstrumentor.addTimer("Section1:Math:RoundDouble", "operations");

    time = 0.0; 
    size = INITSIZE;
    l1=1;l2=2;l3=3;l4=4; 
    
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Math:RoundDouble"); 
      JGFInstrumentor.startTimer("Section1:Math:RoundDouble"); 
      for (i=0; i<size; i++){
          l1=Math.round((double) l2); l2=Math.round((double) l3); l1=Math.round((double) l4); l4=Math.round((double) l1);
          l1=Math.round((double) l2); l2=Math.round((double) l3); l1=Math.round((double) l4); l4=Math.round((double) l1);
          l1=Math.round((double) l2); l2=Math.round((double) l3); l1=Math.round((double) l4); l4=Math.round((double) l1);
          l1=Math.round((double) l2); l2=Math.round((double) l3); l1=Math.round((double) l4); l4=Math.round((double) l1);
        }
      JGFInstrumentor.stopTimer("Section1:Math:RoundDouble"); 

      // try to defeat dead code elimination 
      if (l1 == -1) System.out.println(l1); 
      time = JGFInstrumentor.readTimer("Section1:Math:RoundDouble"); 
      JGFInstrumentor.addOpsToTimer("Section1:Math:RoundDouble", (double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Math:RoundDouble"); 

    JGFInstrumentor.addTimer("Section1:Math:IEEERemainderDouble", "operations");

    time = 0.0; 
    size = INITSIZE;
    d1=1.0D;d2=2.0D;d3=3.0D;d4=4.0D; 
    
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Math:IEEERemainderDouble"); 
      JGFInstrumentor.startTimer("Section1:Math:IEEERemainderDouble"); 
      for (i=0; i<size; i++){
          d1=Math.IEEEremainder(d2,d3); d2=Math.IEEEremainder(d3,d4); d3=Math.IEEEremainder(d4,d1); d4=Math.IEEEremainder(d1,d2);
          d1=Math.IEEEremainder(d2,d3); d2=Math.IEEEremainder(d3,d4); d3=Math.IEEEremainder(d4,d1); d4=Math.IEEEremainder(d1,d2);
          d1=Math.IEEEremainder(d2,d3); d2=Math.IEEEremainder(d3,d4); d3=Math.IEEEremainder(d4,d1); d4=Math.IEEEremainder(d1,d2);
          d1=Math.IEEEremainder(d2,d3); d2=Math.IEEEremainder(d3,d4); d3=Math.IEEEremainder(d4,d1); d4=Math.IEEEremainder(d1,d2);
        }
      JGFInstrumentor.stopTimer("Section1:Math:IEEERemainderDouble"); 

      // try to defeat dead code elimination 
      if (d1 == -1.0D) System.out.println(d1); 
      time = JGFInstrumentor.readTimer("Section1:Math:IEEERemainderDouble"); 
      JGFInstrumentor.addOpsToTimer("Section1:Math:IEEERemainderDouble", (double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Math:IEEERemainderDouble"); 
  }
  
public static void main (String argv[]){

    JGFInstrumentor.printHeader(1,0);

    JGFMathBench mb = new JGFMathBench(); 
    mb.JGFrun();
    
  }
}

