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

public class JGFArithBench implements JGFSection1{


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

    JGFInstrumentor.addTimer("Section1:Arith:Add:Int", "adds");

    time = 0.0; 
    size = INITSIZE; 
    i1=1; i2=-2; i3=3; i4=-4;


    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Arith:Add:Int"); 
      JGFInstrumentor.startTimer("Section1:Arith:Add:Int"); 
      for (i=0; i<size; i++){
	i2+=i1; 
        i3+=i2;
        i4+=i3; 
        i1+=i4;
	i2+=i1; 
        i3+=i2;
        i4+=i3; 
        i1+=i4;
	i2+=i1; 
        i3+=i2;
        i4+=i3; 
        i1+=i4;
	i2+=i1; 
        i3+=i2;
        i4+=i3; 
        i1+=i4;
      }
      JGFInstrumentor.stopTimer("Section1:Arith:Add:Int"); 
      // try to defeat dead code elimination 
      if (i1 == i2) System.out.println(i1); 
      time = JGFInstrumentor.readTimer("Section1:Arith:Add:Int"); 
      JGFInstrumentor.addOpsToTimer("Section1:Arith:Add:Int", (double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Arith:Add:Int"); 


    JGFInstrumentor.addTimer("Section1:Arith:Add:Long", "adds");

    time = 0.0; 
    size = INITSIZE; 
    l1=1L; l2=-2L; l3=3L; l4=-4L;  
 
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Arith:Add:Long"); 
      JGFInstrumentor.startTimer("Section1:Arith:Add:Long"); 
      for (i=0; i<size; i++){
	l2+=l1; 
        l3+=l2;
        l4+=l3; 
        l1+=l4;
	l2+=l1; 
        l3+=l2;
        l4+=l3; 
        l1+=l4;
	l2+=l1; 
        l3+=l2;
        l4+=l3; 
        l1+=l4;
	l2+=l1; 
        l3+=l2;
        l4+=l3; 
        l1+=l4;
      }
      JGFInstrumentor.stopTimer("Section1:Arith:Add:Long"); 

      // try to defeat dead code elimination 
      if (l1 == l2) System.out.println(i1);  /* fix: Sidney */

      time = JGFInstrumentor.readTimer("Section1:Arith:Add:Long"); 
      JGFInstrumentor.addOpsToTimer("Section1:Arith:Add:Long", (double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Arith:Add:Long"); 



    JGFInstrumentor.addTimer("Section1:Arith:Add:Float", "adds");

    time = 0.0; 
    size = INITSIZE; 
    f1=1.0F; f2=-2.0F; f3=3.0F; f4=-4.0F; 
    
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Arith:Add:Float"); 
      JGFInstrumentor.startTimer("Section1:Arith:Add:Float"); 
      for (i=0; i<size; i++){
	f2+=f1; 
        f3+=f2;
        f4+=f3; 
        f1+=f4;
	f2+=f1; 
        f3+=f2;
        f4+=f3; 
        f1+=f4;
	f2+=f1; 
        f3+=f2;
        f4+=f3; 
        f1+=f4;
	f2+=f1; 
        f3+=f2;
        f4+=f3; 
        f1+=f4;
     }
      JGFInstrumentor.stopTimer("Section1:Arith:Add:Float"); 

      // try to defeat dead code elimination 
      if (f1 == 0.5) System.out.println(f1);
 
      time = JGFInstrumentor.readTimer("Section1:Arith:Add:Float"); 
      JGFInstrumentor.addOpsToTimer("Section1:Arith:Add:Float",(double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Arith:Add:Float"); 


    JGFInstrumentor.addTimer("Section1:Arith:Add:Double", "adds");

    time = 0.0; 
    size = INITSIZE; 
    d1=1.0D; d2=-2.0D; d3=3.0D; d4=-4.0D;
   
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Arith:Add:Double"); 
      JGFInstrumentor.startTimer("Section1:Arith:Add:Double"); 
      for (i=0; i<size; i++){
	d2+=d1; 
        d3+=d2;
        d4+=d3; 
        d1+=d4;
	d2+=d1; 
        d3+=d2;
        d4+=d3; 
        d1+=d4;
	d2+=d1; 
        d3+=d2;
        d4+=d3; 
        d1+=d4;
	d2+=d1; 
        d3+=d2;
        d4+=d3; 
        d1+=d4;
      }
      JGFInstrumentor.stopTimer("Section1:Arith:Add:Double"); 

      // try to defeat dead code elimination 
      if (d1 == 0.5) System.out.println(d1); 

      time = JGFInstrumentor.readTimer("Section1:Arith:Add:Double"); 
      JGFInstrumentor.addOpsToTimer("Section1:Arith:Add:Double",(double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Arith:Add:Double"); 




    JGFInstrumentor.addTimer("Section1:Arith:Mult:Int", "multiplies");

    time = 0.0; 
    size = INITSIZE; 
    i1=1; i2=-2; i3=3; i4=-4;
    
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Arith:Mult:Int"); 
      JGFInstrumentor.startTimer("Section1:Arith:Mult:Int"); 
      for (i=0; i<size; i++){
	i2*=i1; 
        i3*=i2;
        i4*=i3; 
        i1*=i4;
	i2*=i1; 
        i3*=i2;
        i4*=i3; 
        i1*=i4;
	i2*=i1; 
        i3*=i2;
        i4*=i3; 
        i1*=i4;
	i2*=i1; 
        i3*=i2;
        i4*=i3; 
        i1*=i4;
      }
      JGFInstrumentor.stopTimer("Section1:Arith:Mult:Int"); 

      // try to defeat dead code elimination 
      if (i1 == i2-1) System.out.println(i1);
 
      time = JGFInstrumentor.readTimer("Section1:Arith:Mult:Int"); 
      JGFInstrumentor.addOpsToTimer("Section1:Arith:Mult:Int", (double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Arith:Mult:Int"); 


    JGFInstrumentor.addTimer("Section1:Arith:Mult:Long", "multiplies");

    time = 0.0; 
    size = INITSIZE; 
    l1=1L; l2=-2L; l3=3L; l4=-4L;  
    
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Arith:Mult:Long"); 
      JGFInstrumentor.startTimer("Section1:Arith:Mult:Long"); 
      for (i=0; i<size; i++){
	l2*=l1; 
        l3*=l2;
        l4*=l3; 
        l1*=l4;
	l2*=l1; 
        l3*=l2;
        l4*=l3; 
        l1*=l4;
	l2*=l1; 
        l3*=l2;
        l4*=l3; 
        l1*=l4;
	l2*=l1; 
        l3*=l2;
        l4*=l3; 
        l1*=l4;
      }
      JGFInstrumentor.stopTimer("Section1:Arith:Mult:Long"); 

      // try to defeat dead code elimination 
      if (l1 == l2-1) System.out.println(l1); 

      time = JGFInstrumentor.readTimer("Section1:Arith:Mult:Long"); 
      JGFInstrumentor.addOpsToTimer("Section1:Arith:Mult:Long", (double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Arith:Mult:Long"); 



    JGFInstrumentor.addTimer("Section1:Arith:Mult:Float", "multiplies");

    time = 0.0; 
    size = INITSIZE; 
    // prevent overflow 
    f2= (float) Math.PI;
    f3= 1.0F/f2;

    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Arith:Mult:Float"); 
      JGFInstrumentor.startTimer("Section1:Arith:Mult:Float"); 
      for (i=0; i<size; i++){
 	f1*=f2;
        f1*=f3;
 	f1*=f2;
        f1*=f3;
 	f1*=f2;
        f1*=f3;
 	f1*=f2;
        f1*=f3;
 	f1*=f2;
        f1*=f3;
 	f1*=f2;
        f1*=f3;
 	f1*=f2;
        f1*=f3;
 	f1*=f2;
        f1*=f3;
      }
      JGFInstrumentor.stopTimer("Section1:Arith:Mult:Float"); 

      // try to defeat dead code elimination 
      if (f1 == 0.5) System.out.println(f1);
 
      time = JGFInstrumentor.readTimer("Section1:Arith:Mult:Float"); 
      JGFInstrumentor.addOpsToTimer("Section1:Arith:Mult:Float",(double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Arith:Mult:Float"); 


    JGFInstrumentor.addTimer("Section1:Arith:Mult:Double", "multiplies");

    time = 0.0; 
    size = INITSIZE; 
    // prevent overflow
    d2 = Math.PI;
    d3 = 1.0D/d3; 
    
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Arith:Mult:Double"); 
      JGFInstrumentor.startTimer("Section1:Arith:Mult:Double"); 
      for (i=0; i<size; i++){
 	d1*=d2;
        d1*=d3;
 	d1*=d2;
        d1*=d3;
 	d1*=d2;
        d1*=d3;
 	d1*=d2;
        d1*=d3;
 	d1*=d2;
        d1*=d3;
 	d1*=d2;
        d1*=d3;
 	d1*=d2;
        d1*=d3;
 	d1*=d2;
        d1*=d3;
       }
      JGFInstrumentor.stopTimer("Section1:Arith:Mult:Double"); 

      // try to defeat dead code elimination 
      if (d1 == 0.5) System.out.println(d1);
 
      time = JGFInstrumentor.readTimer("Section1:Arith:Mult:Double"); 
      JGFInstrumentor.addOpsToTimer("Section1:Arith:Mult:Double",(double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Arith:Mult:Double"); 




    JGFInstrumentor.addTimer("Section1:Arith:Div:Int", "divides");

    time = 0.0; 
    size = INITSIZE; 
    i1 = Integer.MAX_VALUE;
    i2 = 3;
    
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Arith:Div:Int"); 
      JGFInstrumentor.startTimer("Section1:Arith:Div:Int"); 
      for (i=0; i<size; i++){
	i1/=i2;
	i1/=i2;
	i1/=i2;
	i1/=i2;
	i1/=i2;
	i1/=i2;
	i1/=i2;
	i1/=i2;
	i1/=i2;
	i1/=i2;
	i1/=i2;
	i1/=i2;
	i1/=i2;
	i1/=i2;
	i1/=i2;
	i1/=i2;
        if (i1 == 0) i1 = Integer.MAX_VALUE; 
      }
      JGFInstrumentor.stopTimer("Section1:Arith:Div:Int"); 

      // try to defeat dead code elimination 
      if (i1 == -1) System.out.println(i1); 

      time = JGFInstrumentor.readTimer("Section1:Arith:Div:Int"); 
      JGFInstrumentor.addOpsToTimer("Section1:Arith:Div:Int",(double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Arith:Div:Int"); 


    JGFInstrumentor.addTimer("Section1:Arith:Div:Long", "divides");

    time = 0.0; 
    size = INITSIZE; 
    l1 = Long.MAX_VALUE;
    l2 = 3L; 
    
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Arith:Div:Long"); 
      JGFInstrumentor.startTimer("Section1:Arith:Div:Long"); 
      for (i=0; i<size; i++){
	l1/=l2;
	l1/=l2;
	l1/=l2;
	l1/=l2;
	l1/=l2;
	l1/=l2;
	l1/=l2;
	l1/=l2;
	l1/=l2;
	l1/=l2;
	l1/=l2;
	l1/=l2;
	l1/=l2;
	l1/=l2;
	l1/=l2;
	l1/=l2;
        if (l1 == 0) l1 = Long.MAX_VALUE; 
       }
      JGFInstrumentor.stopTimer("Section1:Arith:Div:Long"); 

      // try to defeat dead code elimination 
      if (l1 == -1) System.out.println(l1);
 
      time = JGFInstrumentor.readTimer("Section1:Arith:Div:Long"); 
      JGFInstrumentor.addOpsToTimer("Section1:Arith:Div:Long", (double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Arith:Div:Long"); 



    JGFInstrumentor.addTimer("Section1:Arith:Div:Float", "divides");

    time = 0.0; 
    size = INITSIZE; 
    // prevent overflow 
    f2= (float) Math.PI;
    f3= 1.0F/f2;
    
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Arith:Div:Float"); 
      JGFInstrumentor.startTimer("Section1:Arith:Div:Float"); 
      for (i=0; i<size; i++){
 	f1/=f2;
        f1/=f3;
 	f1/=f2;
        f1/=f3;
 	f1/=f2;
        f1/=f3;
 	f1/=f2;
        f1/=f3;
 	f1/=f2;
        f1/=f3;
 	f1/=f2;
        f1/=f3;
 	f1/=f2;
        f1/=f3;
 	f1/=f2;
        f1/=f3;
      }
      JGFInstrumentor.stopTimer("Section1:Arith:Div:Float"); 

      // try to defeat dead code elimination 
      if (f1 == 0.5) System.out.println(f1);
 
      time = JGFInstrumentor.readTimer("Section1:Arith:Div:Float"); 
      JGFInstrumentor.addOpsToTimer("Section1:Arith:Div:Float", (double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Arith:Div:Float"); 


    JGFInstrumentor.addTimer("Section1:Arith:Div:Double", "divides");

    time = 0.0; 
    size = INITSIZE; 
    // prevent overflow
    d2 = Math.PI;
    d3 = 1.0D/d3; 
    
    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Arith:Div:Double"); 
      JGFInstrumentor.startTimer("Section1:Arith:Div:Double"); 
      for (i=0; i<size; i++){
 	d1/=d2;
        d1/=d3;
 	d1/=d2;
        d1/=d3;
 	d1/=d2;
        d1/=d3;
 	d1/=d2;
        d1/=d3;
 	d1/=d2;
        d1/=d3;
 	d1/=d2;
        d1/=d3;
 	d1/=d2;
        d1/=d3;
 	d1/=d2;
        d1/=d3;
      }
      JGFInstrumentor.stopTimer("Section1:Arith:Div:Double"); 

      // try to defeat dead code elimination 
      if (d1 == 0.5) System.out.println(d1); 

      time = JGFInstrumentor.readTimer("Section1:Arith:Div:Double"); 
      JGFInstrumentor.addOpsToTimer("Section1:Arith:Div:Double",(double) 16*size);  
      size *=2; 
    }
   
    JGFInstrumentor.printperfTimer("Section1:Arith:Div:Double"); 

  }


  public static void main (String argv[]){

    JGFInstrumentor.printHeader(1,0);

    JGFArithBench ab = new JGFArithBench(); 
    ab.JGFrun();
    
  }

}   



