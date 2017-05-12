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


import java.io.*;
import jgfutil.*;
import java.util.Vector;

public class JGFSerialBench implements JGFSection1 {

  public static final int INITSIZE = 10;
  public static final int MAXSIZE = 10000;
  private static final double TARGETTIME = 10.0;
  public static int listsize= 0;
  public static final int LENGTH = 100;
  public static int depthsize= 0;
  public static final int MAXDEPTH = 10;

  public void JGFrun(){

    int i,j,size;
    double time;
    item base [] = null; 
    ObjectOutputStream out = null;
    ObjectInputStream in = null;
    FileInputStream fin = null;

    JGFInstrumentor.addTimer("Section1:Serial:Writing:Linklist", "bytes");
    time = 0.0;
    size = INITSIZE;

    File serialfile1 = new File("serial1.out");

    while  (time < TARGETTIME && size < MAXSIZE){
      base = new item [size];
      
      for (i=0; i<size; i++) {
       listsize = 0;
       base[i] = new item();
       base[i].add();
      }

      try{
       FileOutputStream fout = new FileOutputStream(serialfile1);
       out = new ObjectOutputStream(fout); 
      }

      catch (IOException e) {};

      JGFInstrumentor.resetTimer("Section1:Serial:Writing:Linklist");
      JGFInstrumentor.startTimer("Section1:Serial:Writing:Linklist");

      try{
        for (i=0; i<size; i++) {
        out.writeObject(base[i]); 
        }
      }

      catch (IOException e) {}; 

      JGFInstrumentor.stopTimer("Section1:Serial:Writing:Linklist");
      time = JGFInstrumentor.readTimer("Section1:Serial:Writing:Linklist");

    JGFInstrumentor.addOpsToTimer("Section1:Serial:Writing:Linklist", (double) 
    size*4*listsize);

      try{
        out.flush();
        out.close();
      }

      catch (IOException e) {};

      size *=2;

    }

    JGFInstrumentor.printperfTimer("Section1:Serial:Writing:Linklist");

      size /=2;

      try{
      fin = new FileInputStream(serialfile1);
      in = new ObjectInputStream(fin);
      }

      catch (Exception e) {};

      JGFInstrumentor.addTimer("Section1:Serial:Reading:Linklist", "bytes");
      JGFInstrumentor.resetTimer("Section1:Serial:Reading:Linklist");
      JGFInstrumentor.startTimer("Section1:Serial:Reading:Linklist");


      try{
        for (i=0; i<size; i++) {
        base[i] = (item) in.readObject();
        }
      }

      catch (Exception e) {};

      JGFInstrumentor.stopTimer("Section1:Serial:Reading:Linklist");
      time = JGFInstrumentor.readTimer("Section1:Serial:Reading:Linklist");

      JGFInstrumentor.addOpsToTimer("Section1:Serial:Reading:Linklist", (double) 
      size*4*listsize);

      try{
        in.close();
      }
     
      catch (Exception e) {};
   
      serialfile1.delete();
 
      JGFInstrumentor.printperfTimer("Section1:Serial:Reading:Linklist");

// Start of Binarytree

    itemtree basetree [] = null; 
    ObjectOutputStream outtree = null;
    ObjectInputStream intree = null;
    FileInputStream fintree = null;

    JGFInstrumentor.addTimer("Section1:Serial:Writing:Binarytree", "bytes");
    time = 0.0;
    size = INITSIZE;

    File serialfile2 = new File("serial2.out");

    while  (time < TARGETTIME && size < MAXSIZE){
      basetree = new itemtree [size];
      
      for (i=0; i<size; i++) {
       depthsize = 0;
       basetree[i] = new itemtree();
       basetree[i].add();
      }

      try{
       FileOutputStream fouttree = new FileOutputStream(serialfile2);
       outtree = new ObjectOutputStream(fouttree); 
      }

      catch (IOException e) {};

      JGFInstrumentor.resetTimer("Section1:Serial:Writing:Binarytree");
      JGFInstrumentor.startTimer("Section1:Serial:Writing:Binarytree");

      try{
        for (i=0; i<size; i++) {
        outtree.writeObject(basetree[i]); 
        }
      }

      catch (IOException e) {}; 

      JGFInstrumentor.stopTimer("Section1:Serial:Writing:Binarytree");
      time = JGFInstrumentor.readTimer("Section1:Serial:Writing:Binarytree");

      JGFInstrumentor.addOpsToTimer("Section1:Serial:Writing:Binarytree", (double)
      size*4*(Math.pow(2,(MAXDEPTH+1))-1));

      try{
        outtree.flush();
        outtree.close();
      }

      catch (IOException e) {};

      size *=2;

    }

    JGFInstrumentor.printperfTimer("Section1:Serial:Writing:Binarytree");

      size /=2;

      try{
      fintree = new FileInputStream(serialfile2);
      intree = new ObjectInputStream(fintree);
      }

      catch (Exception e) {};

      JGFInstrumentor.addTimer("Section1:Serial:Reading:Binarytree", "bytes");
      JGFInstrumentor.resetTimer("Section1:Serial:Reading:Binarytree");
      JGFInstrumentor.startTimer("Section1:Serial:Reading:Binarytree");


      try{
        for (i=0; i<size; i++) {
        basetree[i] = (itemtree) intree.readObject();
        }
      }

      catch (Exception e) {};

      JGFInstrumentor.stopTimer("Section1:Serial:Reading:Binarytree");
      time = JGFInstrumentor.readTimer("Section1:Serial:Reading:Binarytree");
      JGFInstrumentor.addOpsToTimer("Section1:Serial:Reading:Binarytree", (double) 
      size*4*(Math.pow(2,(MAXDEPTH+1))-1));

      try{
        intree.close();
      }

      catch (Exception e) {};

      serialfile2.delete();

      JGFInstrumentor.printperfTimer("Section1:Serial:Reading:Binarytree");

// end of Binarytree 

// start of vector

    itemvector basevector = null; 
    ObjectOutputStream outvector = null;
    ObjectInputStream invector = null;
    FileInputStream finvector = null;
    Vector vect [] = null;
 
    JGFInstrumentor.addTimer("Section1:Serial:Writing:Vector", "bytes");
    time = 0.0;
    size = INITSIZE;

    File serialfile3 = new File("serial3.out");

    while  (time < TARGETTIME && size < MAXSIZE){
      basevector = new itemvector ();
      vect = new Vector [size];
    
      for (j=0; j<size; j++) {
        vect[j] = new Vector();
         for (i=0; i<LENGTH; i++) {
           basevector = new itemvector();
           vect[j].addElement(basevector);
         }
      }

      try{
       FileOutputStream foutvector = new FileOutputStream(serialfile3);
       outvector = new ObjectOutputStream(foutvector); 
      }

      catch (IOException e) {};

      JGFInstrumentor.resetTimer("Section1:Serial:Writing:Vector");
      JGFInstrumentor.startTimer("Section1:Serial:Writing:Vector");

      try{
        for (j=0; j<size; j++) {
        outvector.writeObject(vect[j]); 
        }
      }

      catch (IOException e) {}; 

      JGFInstrumentor.stopTimer("Section1:Serial:Writing:Vector");
      time = JGFInstrumentor.readTimer("Section1:Serial:Writing:Vector");

    JGFInstrumentor.addOpsToTimer("Section1:Serial:Writing:Vector", (double) 
    LENGTH*size*4);

      try{
        outvector.flush();
        outvector.close();
      }

      catch (IOException e) {};

      size *=2;

    }

      
    JGFInstrumentor.printperfTimer("Section1:Serial:Writing:Vector");

      size /=2;

      try{
      finvector = new FileInputStream(serialfile3);
      invector = new ObjectInputStream(finvector);
      }

      catch (Exception e) {};

      JGFInstrumentor.addTimer("Section1:Serial:Reading:Vector", "bytes");
      JGFInstrumentor.resetTimer("Section1:Serial:Reading:Vector");
      JGFInstrumentor.startTimer("Section1:Serial:Reading:Vector");

     
      try{
        for (i=0; i<size; i++) {
        vect[i] = (Vector) invector.readObject();
        }
      }

      catch (Exception e) {};

      JGFInstrumentor.stopTimer("Section1:Serial:Reading:Vector");
      time = JGFInstrumentor.readTimer("Section1:Serial:Reading:Vector");

      JGFInstrumentor.addOpsToTimer("Section1:Serial:Reading:Vector", (double) 
      LENGTH*size*4);

      try{
        invector.close();
      }
     
      catch (Exception e) {};
   
      serialfile3.delete();
 
      JGFInstrumentor.printperfTimer("Section1:Serial:Reading:Vector");

// end of vector

// Start of Array

    arrayitem arraybase [][] = null; 
    ObjectOutputStream arrayout = null;
    ObjectInputStream arrayin = null;
    FileInputStream arrayfin = null;
 
    JGFInstrumentor.addTimer("Section1:Serial:Writing:Array", "bytes");
    time = 0.0;
    size = INITSIZE;

    File serialfile4 = new File("serial4.out");

    while  (time < TARGETTIME && size < MAXSIZE){
      arraybase = new arrayitem [size][LENGTH];

      for (j=0; j<size; j++) { 
        for (i=0; i<LENGTH; i++) {
         arraybase[j][i] = new arrayitem();
        }
      }

      try{
       FileOutputStream arrayfout = new FileOutputStream(serialfile4);
       arrayout = new ObjectOutputStream(arrayfout); 
      }

      catch (IOException e) {};

      JGFInstrumentor.resetTimer("Section1:Serial:Writing:Array");
      JGFInstrumentor.startTimer("Section1:Serial:Writing:Array");

      try{
        for (j=0; j<size; j++) {
        arrayout.writeObject( arraybase[j]); 
        }
      }

      catch (IOException e) {}; 

      JGFInstrumentor.stopTimer("Section1:Serial:Writing:Array");
      time = JGFInstrumentor.readTimer("Section1:Serial:Writing:Array");

    JGFInstrumentor.addOpsToTimer("Section1:Serial:Writing:Array", (double) 
    size*LENGTH*4);

      try{
        arrayout.flush();
        arrayout.close();
      }

      catch (IOException e) {};

      size *=2;

    }

      
    JGFInstrumentor.printperfTimer("Section1:Serial:Writing:Array");

      size /=2;

      try{
      arrayfin = new FileInputStream(serialfile4);
      arrayin = new ObjectInputStream(arrayfin);
      }

      catch (Exception e) {};

      JGFInstrumentor.addTimer("Section1:Serial:Reading:Array", "bytes");
      JGFInstrumentor.resetTimer("Section1:Serial:Reading:Array");
      JGFInstrumentor.startTimer("Section1:Serial:Reading:Array");

     
      try{
        for (i=0; i<size; i++) {
        arraybase[i] = (arrayitem []) arrayin.readObject();
        }
      }

      catch (Exception e) {};


      JGFInstrumentor.stopTimer("Section1:Serial:Reading:Array");
      time = JGFInstrumentor.readTimer("Section1:Serial:Reading:Array");

      JGFInstrumentor.addOpsToTimer("Section1:Serial:Reading:Array", (double) 
      size*LENGTH*4);

      try{
        arrayin.close();
      }
     
      catch (Exception e) {};
      
      serialfile4.delete();
 
      JGFInstrumentor.printperfTimer("Section1:Serial:Reading:Array");

// end of Array

  }

  public static void main(String[] argv) {

   JGFSerialBench szb = new JGFSerialBench();
   szb.JGFrun();

  }


}

class item implements Serializable {
  item next;
  int poss;


   public void add() {
    if (JGFSerialBench.listsize < JGFSerialBench.LENGTH){
     next = new item();
     JGFSerialBench.listsize++;
     next.poss = JGFSerialBench.listsize;
     next.add();
    }
   }

}

class itemtree implements Serializable {
  itemtree nextl;
  itemtree nextr;
  int poss;


   public void add() {
    if (JGFSerialBench.depthsize < JGFSerialBench.MAXDEPTH){
     this.poss=JGFSerialBench.depthsize;
     JGFSerialBench.depthsize++;
     nextl = new itemtree();
     nextl.add();
     JGFSerialBench.depthsize++;
     nextr = new itemtree();
     nextr.add();
     }
      JGFSerialBench.depthsize--;
   }

}

class itemvector implements Serializable {
  int poss;

}

class arrayitem implements Serializable {
  int poss;

}


