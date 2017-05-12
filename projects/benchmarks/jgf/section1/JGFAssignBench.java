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

public class JGFAssignBench implements JGFSection1{


  static int a1=1,a2=2,a3=3,a4=4,b[]={1,2,3,4}; 
  private static final int INITSIZE = 10000;
  private static final int MAXSIZE = 1000000000;
  private static final double TARGETTIME = 10.0; 
  

  int c1=1,c2=2,c3=3,c4=4, d[]={1,2,3,4}; 

  public void JGFrun(){

    int i,size;
    double time; 

    int e1=1,e2=2,e3=3,e4=4; 
    int f[] ={1,2,3,4};



    JGFInstrumentor.addTimer("Section1:Assign:Same:Scalar:Local", "assignments");
    time = 0.0; 
    size = INITSIZE; 

    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Assign:Same:Scalar:Local"); 
      JGFInstrumentor.startTimer("Section1:Assign:Same:Scalar:Local"); 
      for (i=0; i<size; i++){
	e1=e2; 
	e2=e3; 
	e3=e4; 
	e4=e1; 
	e1=e2; 
	e2=e3; 
	e3=e4; 
	e4=e1; 
	e1=e2; 
	e2=e3; 
	e3=e4; 
	e4=e1; 
	e1=e2; 
	e2=e3; 
	e3=e4; 
	e4=e1; 
      }
      JGFInstrumentor.stopTimer("Section1:Assign:Same:Scalar:Local");
      // try to defeat dead code elimination 
      if (e4 == e3) System.out.println(e4);
      time = JGFInstrumentor.readTimer("Section1:Assign:Same:Scalar:Local"); 
      JGFInstrumentor.addOpsToTimer("Section1:Assign:Same:Scalar:Local", (double) 16*size);
      size *=2; 
    }
       
    JGFInstrumentor.printperfTimer("Section1:Assign:Same:Scalar:Local");
        


    JGFInstrumentor.addTimer("Section1:Assign:Same:Scalar:Instance", "assignments");
    time = 0.0; 
    size = INITSIZE; 

    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Assign:Same:Scalar:Instance"); 
      JGFInstrumentor.startTimer("Section1:Assign:Same:Scalar:Instance"); 
      for (i=0; i<size; i++){
	c1=c2; 
	c2=c3; 
	c3=c4; 
	c4=c1; 
	c1=c2; 
	c2=c3; 
	c3=c4; 
	c4=c1; 
	c1=c2; 
	c2=c3; 
	c3=c4; 
	c4=c1; 
	c1=c2; 
	c2=c3; 
	c3=c4; 
	c4=c1; 
      }
      JGFInstrumentor.stopTimer("Section1:Assign:Same:Scalar:Instance");
      // try to defeat dead code elimination 
      if (c4 == c3) System.out.println(c4);
      time = JGFInstrumentor.readTimer("Section1:Assign:Same:Scalar:Instance"); 
      JGFInstrumentor.addOpsToTimer("Section1:Assign:Same:Scalar:Instance", (double) 16*size);
      size *=2; 
    }
       
    JGFInstrumentor.printperfTimer("Section1:Assign:Same:Scalar:Instance");
        
    JGFInstrumentor.addTimer("Section1:Assign:Same:Scalar:Class", "assignments");
    time = 0.0; 
    size = INITSIZE; 

    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Assign:Same:Scalar:Class"); 
      JGFInstrumentor.startTimer("Section1:Assign:Same:Scalar:Class"); 
      for (i=0; i<size; i++){
	a1=a2; 
	a2=a3; 
	a3=a4; 
	a4=a1; 
	a1=a2; 
	a2=a3; 
	a3=a4; 
	a4=a1; 
	a1=a2; 
	a2=a3; 
	a3=a4; 
	a4=a1; 
	a1=a2; 
	a2=a3; 
	a3=a4; 
	a4=a1; 
      }
      JGFInstrumentor.stopTimer("Section1:Assign:Same:Scalar:Class");
      // try to defeat dead code elimination 
      if (a3 == a4) System.out.println(a3);
      time = JGFInstrumentor.readTimer("Section1:Assign:Same:Scalar:Class"); 
      JGFInstrumentor.addOpsToTimer("Section1:Assign:Same:Scalar:Class", (double) 16*size);
      size *=2; 
    }
       
    JGFInstrumentor.printperfTimer("Section1:Assign:Same:Scalar:Class");
        
    JGFInstrumentor.addTimer("Section1:Assign:Same:Array:Local", "assignments");
    time = 0.0; 
    size = INITSIZE; 

    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Assign:Same:Array:Local"); 
      JGFInstrumentor.startTimer("Section1:Assign:Same:Array:Local"); 
      for (i=0; i<size; i++){
	f[0]=f[1]; 
	f[1]=f[2]; 
	f[2]=f[3]; 
	f[3]=f[0]; 
	f[0]=f[1]; 
	f[1]=f[2]; 
	f[2]=f[3]; 
	f[3]=f[0]; 
	f[0]=f[1]; 
	f[1]=f[2]; 
	f[2]=f[3]; 
	f[3]=f[0]; 
	f[0]=f[1]; 
	f[1]=f[2]; 
	f[2]=f[3]; 
	f[3]=f[0]; 
      }
      JGFInstrumentor.stopTimer("Section1:Assign:Same:Array:Local");
      // try to defeat dead code elimination 
      if (f[3] == f[2]) System.out.println(f[3]);
      time = JGFInstrumentor.readTimer("Section1:Assign:Same:Array:Local"); 
      JGFInstrumentor.addOpsToTimer("Section1:Assign:Same:Array:Local", (double) 16*size);
      size *=2; 
    }
       
    JGFInstrumentor.printperfTimer("Section1:Assign:Same:Array:Local");


    JGFInstrumentor.addTimer("Section1:Assign:Same:Array:Instance", "assignments");
    time = 0.0; 
    size = INITSIZE; 

    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Assign:Same:Array:Instance"); 
      JGFInstrumentor.startTimer("Section1:Assign:Same:Array:Instance"); 
      for (i=0; i<size; i++){
        d[0]=d[1];
	d[1]=d[2]; 
	d[2]=d[3]; 
	d[3]=d[0]; 
        d[0]=d[1];
	d[1]=d[2]; 
	d[2]=d[3]; 
	d[3]=d[0]; 
        d[0]=d[1];
	d[1]=d[2]; 
	d[2]=d[3]; 
	d[3]=d[0]; 
        d[0]=d[1];
	d[1]=d[2]; 
	d[2]=d[3]; 
	d[3]=d[0]; 
        d[0]=d[1];
	d[1]=d[2]; 
	d[2]=d[3]; 
	d[3]=d[0]; 
        d[0]=d[1];
	d[1]=d[2]; 
	d[2]=d[3]; 
	d[3]=d[0]; 
        d[0]=d[1];
	d[1]=d[2]; 
	d[2]=d[3]; 
	d[3]=d[0]; 
        d[0]=d[1];
	d[1]=d[2]; 
	d[2]=d[3]; 
	d[3]=d[0]; 
      }
      JGFInstrumentor.stopTimer("Section1:Assign:Same:Array:Instance");
      // try to defeat dead code elimination 
      if (d[3] == d[2]) System.out.println(d[3]);
      time = JGFInstrumentor.readTimer("Section1:Assign:Same:Array:Instance"); 
      JGFInstrumentor.addOpsToTimer("Section1:Assign:Same:Array:Instance", (double) 32*size); /* fix: Sidney */
      size *=2; 
    }
       
    JGFInstrumentor.printperfTimer("Section1:Assign:Same:Array:Instance");
        
    JGFInstrumentor.addTimer("Section1:Assign:Same:Array:Class", "assignments");
    time = 0.0; 
    size = INITSIZE; 

    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Assign:Same:Array:Class"); 
      JGFInstrumentor.startTimer("Section1:Assign:Same:Array:Class"); 
      for (i=0; i<size; i++){
	b[0]=b[1]; 
	b[1]=b[2]; 
	b[2]=b[3]; 
	b[3]=b[0]; 
	b[0]=b[1]; 
	b[1]=b[2]; 
	b[2]=b[3]; 
	b[3]=b[0]; 
	b[0]=b[1]; 
	b[1]=b[2]; 
	b[2]=b[3]; 
	b[3]=b[0]; 
	b[0]=b[1]; 
	b[1]=b[2]; 
	b[2]=b[3]; 
	b[3]=b[0]; 
      }
      JGFInstrumentor.stopTimer("Section1:Assign:Same:Array:Class");
      // try to defeat dead code elimination 
      if (b[3] == b[2]) System.out.println(b[3]);
      time = JGFInstrumentor.readTimer("Section1:Assign:Same:Array:Class"); 
      JGFInstrumentor.addOpsToTimer("Section1:Assign:Same:Array:Class", (double) 16*size);
      size *=2; 
    }
       
    JGFInstrumentor.printperfTimer("Section1:Assign:Same:Array:Class");





    AssignTester at = new AssignTester();
        

    JGFInstrumentor.addTimer("Section1:Assign:Other:Scalar:Instance", "assignments");
    time = 0.0; 
    size = INITSIZE; 

    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Assign:Other:Scalar:Instance"); 
      JGFInstrumentor.startTimer("Section1:Assign:Other:Scalar:Instance"); 
      for (i=0; i<size; i++){
	at.c1=at.c2; 
	at.c2=at.c3; 
	at.c3=at.c4; 
	at.c4=at.c1; 
	at.c1=at.c2; 
	at.c2=at.c3; 
	at.c3=at.c4; 
	at.c4=at.c1; 
	at.c1=at.c2; 
	at.c2=at.c3; 
	at.c3=at.c4; 
	at.c4=at.c1; 
	at.c1=at.c2; 
	at.c2=at.c3; 
	at.c3=at.c4; 
	at.c4=at.c1; 
      }
      JGFInstrumentor.stopTimer("Section1:Assign:Other:Scalar:Instance");
      // try to defeat dead code elimination 
      if (at.c3 == at.c4) System.out.println(at.c3);
      time = JGFInstrumentor.readTimer("Section1:Assign:Other:Scalar:Instance"); 
      JGFInstrumentor.addOpsToTimer("Section1:Assign:Other:Scalar:Instance", (double) 16*size);
      size *=2; 
    }
       
    JGFInstrumentor.printperfTimer("Section1:Assign:Other:Scalar:Instance");
        
    JGFInstrumentor.addTimer("Section1:Assign:Other:Scalar:Class", "assignments");
    time = 0.0; 
    size = INITSIZE; 

    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Assign:Other:Scalar:Class"); 
      JGFInstrumentor.startTimer("Section1:Assign:Other:Scalar:Class"); 
      for (i=0; i<size; i++){
	AssignTester.a1=AssignTester.a2; 
	AssignTester.a2=AssignTester.a3; 
	AssignTester.a3=AssignTester.a4; 
	AssignTester.a4=AssignTester.a1; 
	AssignTester.a1=AssignTester.a2; 
	AssignTester.a2=AssignTester.a3; 
	AssignTester.a3=AssignTester.a4; 
	AssignTester.a4=AssignTester.a1; 
	AssignTester.a1=AssignTester.a2; 
	AssignTester.a2=AssignTester.a3; 
	AssignTester.a3=AssignTester.a4; 
	AssignTester.a4=AssignTester.a1; 
	AssignTester.a1=AssignTester.a2; 
	AssignTester.a2=AssignTester.a3; 
	AssignTester.a3=AssignTester.a4; 
	AssignTester.a4=AssignTester.a1; 
      }
      JGFInstrumentor.stopTimer("Section1:Assign:Other:Scalar:Class");
      // try to defeat dead code elimination 
      if (AssignTester.a3 ==AssignTester.a4 ) System.out.println(AssignTester.a3);
      time = JGFInstrumentor.readTimer("Section1:Assign:Other:Scalar:Class"); 
      JGFInstrumentor.addOpsToTimer("Section1:Assign:Other:Scalar:Class", (double) 16*size);
      size *=2; 
    }
       
    JGFInstrumentor.printperfTimer("Section1:Assign:Other:Scalar:Class");
        

    JGFInstrumentor.addTimer("Section1:Assign:Other:Array:Instance", "assignments");
    time = 0.0; 
    size = INITSIZE; 

    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Assign:Other:Array:Instance"); 
      JGFInstrumentor.startTimer("Section1:Assign:Other:Array:Instance"); 
      for (i=0; i<size; i++){
	at.d[0]=at.d[1]; 
	at.d[1]=at.d[2]; 
	at.d[2]=at.d[3]; 
	at.d[3]=at.d[0]; 
	at.d[0]=at.d[1]; 
	at.d[1]=at.d[2]; 
	at.d[2]=at.d[3]; 
	at.d[3]=at.d[0]; 
	at.d[0]=at.d[1]; 
	at.d[1]=at.d[2]; 
	at.d[2]=at.d[3]; 
	at.d[3]=at.d[0]; 
	at.d[0]=at.d[1]; 
	at.d[1]=at.d[2]; 
	at.d[2]=at.d[3]; 
	at.d[3]=at.d[0]; 
      }
      JGFInstrumentor.stopTimer("Section1:Assign:Other:Array:Instance");
      // try to defeat dead code elimination 
      if (at.d[3] == at.d[2]) System.out.println(at.d[3]);
      time = JGFInstrumentor.readTimer("Section1:Assign:Other:Array:Instance"); 
      JGFInstrumentor.addOpsToTimer("Section1:Assign:Other:Array:Instance", (double) 16*size);
      size *=2; 
    }
       
    JGFInstrumentor.printperfTimer("Section1:Assign:Other:Array:Instance");
        
    JGFInstrumentor.addTimer("Section1:Assign:Other:Array:Class", "assignments");
    time = 0.0; 
    size = INITSIZE; 

    while (time < TARGETTIME && size < MAXSIZE){
      JGFInstrumentor.resetTimer("Section1:Assign:Other:Array:Class"); 
      JGFInstrumentor.startTimer("Section1:Assign:Other:Array:Class"); 
      for (i=0; i<size; i++){
	AssignTester.b[0]=AssignTester.b[1]; 
	AssignTester.b[1]=AssignTester.b[2]; 
	AssignTester.b[2]=AssignTester.b[3]; 
	AssignTester.b[3]=AssignTester.b[0]; 
 	AssignTester.b[0]=AssignTester.b[1]; 
	AssignTester.b[1]=AssignTester.b[2]; 
	AssignTester.b[2]=AssignTester.b[3]; 
	AssignTester.b[3]=AssignTester.b[0]; 
	AssignTester.b[0]=AssignTester.b[1]; 
	AssignTester.b[1]=AssignTester.b[2]; 
	AssignTester.b[2]=AssignTester.b[3]; 
	AssignTester.b[3]=AssignTester.b[0]; 
 	AssignTester.b[0]=AssignTester.b[1]; 
	AssignTester.b[1]=AssignTester.b[2]; 
	AssignTester.b[2]=AssignTester.b[3]; 
	AssignTester.b[3]=AssignTester.b[0]; 
      }
      JGFInstrumentor.stopTimer("Section1:Assign:Other:Array:Class");
      // try to defeat dead code elimination 
      if (AssignTester.b[3] == AssignTester.b[2]) System.out.println(AssignTester.b[3]);
      time = JGFInstrumentor.readTimer("Section1:Assign:Other:Array:Class"); 
      JGFInstrumentor.addOpsToTimer("Section1:Assign:Other:Array:Class", (double) 16*size);
      size *=2; 
    }
       
    JGFInstrumentor.printperfTimer("Section1:Assign:Other:Array:Class");
        
  }
  public static void main (String argv[]){

    JGFInstrumentor.printHeader(1,0);

    JGFAssignBench asb = new JGFAssignBench(); 
    asb.JGFrun();
    
  }


}


class AssignTester {

static int a1=1,a2=2,a3=3,a4=4,b[]={1,2,3,4};
int c1=1,c2=2,c3=3,c4=4, d[]={1,2,3,4};


}
