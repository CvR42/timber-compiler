
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

public interface ConnectFourConstants {
  public final int TRANSIZE = 1050011;
  public final int PROBES = 8;
  public final int REPORTPLY = 8;

  public final int UNK = -4;
  public final int LOSE = -2;
  public final int DRAWLOSE = -1;
  public final int DRAW = 0;
  public final int DRAWWIN = 1;
  public final int WIN = 2;
  public final int EMPTY = 0;
  public final int BLACK = 1;
  public final int WHITE = 2;
  public final int EDGE = 3;

  public final String startingMoves[]={"444333377","44433337"};
}
