
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
*                  Original version of this code by                       *
*                     John Tromp (tromp@cwi.nl)                           *
*                   (see copyright notice below)                          *
*                                                                         *
*      This version copyright (c) The University of Edinburgh, 1999.      *
*                         All rights reserved.                            *
*                                                                         *
**************************************************************************/


// Java Fhourstones 2.0 connect-4 solver
// (http://www.cwi.nl/~tromp/c4/fhour.html)
//
// implementation of the well-known game
// played on a vertical board of 7 columns by 6 rows,
// where 2 players take turns in dropping counters in a column.
// the first player to get four of his counters
// in a horizontal, vertical or diagonal row, wins the game.
// if neither player has won after 42 moves, then the game is drawn.
//
// This software is copyright (c) 1996 by
//      John Tromp
//      Lindenlaan 33
//      1701 GT Heerhugowaard
//      Netherlands
// E-mail: tromp@cwi.nl
//
// This notice must not be removed.
// This software must not be sold for profit.
// You may redistribute if your distributees have the
// same rights and restrictions.
 
package search; 

public class Game {
  static int colthr[];
  static boolean colwon[];

  static {
    int i;

    colthr = new int[128];
    for (i=8; i<128; i+=8) {
      colthr[i] = 1;
      colthr[i+7] = 2;
    }
    colwon = new boolean[128];
    for (i=16; i<128; i+=16)
      colwon[i] = colwon[i+15] = true;
  }
  
  int moves[],plycnt;
  int rows[], dias[];
  int columns[], height[];
  
  public Game()
  {
    super(); 
    moves = new int[44];
    rows = new int[8];
    dias = new int[19];
    columns = new int[8];
    height = new int[8];
    reset();
  }
  
  void reset()
  {
    plycnt = 0;
    for (int i=0; i<19; i++)
      dias[i] = 0;
    for (int i=0; i<8; i++) {
      columns[i] = 1;
      height[i] = 1;
      rows[i] = 0;
    }
  }

  public String toString()
  {
    StringBuffer buf = new StringBuffer();

    for (int i=1; i<=plycnt; i++)
      buf.append(moves[i]);
    return buf.toString();
  }

  // return whether a move by player side in column n at height h would win
  // non-vertically
  final boolean wins(int n, int h, int sidemask)
  { 
    int x,y;

    sidemask <<= (2*n);
    x = rows[h] | sidemask;
    y = x & (x<<2);
    if ((y & (y<<4)) != 0)
      return true;
    x = dias[5+n+h] | sidemask;
    y = x & (x<<2);
    if ((y & (y<<4)) != 0)
      return true;
    x = dias[5+n-h] | sidemask;
    y = x & (x<<2);
    return (y & (y<<4)) != 0;
  }

  void backmove()
  {
    int mask,d,h,n,side;

    side = plycnt&1;
    n = moves[plycnt--];
    h = --height[n];
    columns[n] >>= 1;
    mask = ~(1 << (2*n + side));
    rows[h] &= mask;
    dias[5+n+h] &= mask;
    dias[5+n-h] &= mask;
  }

  void makemove(int n) 
  {
    int mask,d,h,side;
  
    moves[++plycnt] = n;
    side = plycnt&1;
    h = height[n]++;
    columns[n] = (columns[n] << 1) + side;
    mask = 1 << (2*n + side);
    rows[h] |= mask;
    dias[5+n+h] |= mask;
    dias[5+n-h] |= mask;
  }
}
