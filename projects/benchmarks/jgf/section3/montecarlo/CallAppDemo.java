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
*      Original version of this code by Hon Yau (hwyau@epcc.ed.ac.uk)     *
*                                                                         *
*      This version copyright (c) The University of Edinburgh, 1999.      *
*                         All rights reserved.                            *
*                                                                         *
**************************************************************************/



package montecarlo;

/**
  * Wrapper code to invoke the Application demonstrator.
  *
  * @author H W Yau
  * @version $Revision: 1.1 $ $Date: 2000/06/28 17:01:26 $
  */
public class CallAppDemo {
    public int size;
    int datasizes[] = {10000,60000};
    int input[] = new int[2];
    AppDemo ap = null;

    public void initialise () {

      input[0] = 1000;
      input[1] = datasizes[size];

      String dirName="Data";
      String filename="hitData";
      ap = new AppDemo(dirName, filename,
      (input[0]),(input[1]));
      ap.initSerial();
    }

    public void runiters () {
      ap.runSerial();
    }
    public void presults () {
      ap.processSerial();
    }

}
