
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
import jgfutil.*; 

public class SearchGame extends TransGame {

  int history[][] = { {-1,-1,-1,-1,-1,-1,-1,-1,
		-1, 0, 1, 2, 4, 2, 1, 0,
		-1, 1, 3, 5, 7, 5, 3, 1,
		-1, 2, 5, 8,10, 8, 5, 2,
		-1, 2, 5, 8,10, 8, 5, 2,
		-1, 1, 3, 5, 7, 5, 3, 1,
		-1, 0, 1, 2, 4, 2, 1, 0},
		    {-1,-1,-1,-1,-1,-1,-1,-1,
		-1, 0, 1, 2, 4, 2, 1, 0,
		-1, 1, 3, 5, 7, 5, 3, 1,
		-1, 2, 5, 8,10, 8, 5, 2,
		-1, 2, 5, 8,10, 8, 5, 2,
		-1, 1, 3, 5, 7, 5, 3, 1,
		-1, 0, 1, 2, 4, 2, 1, 0}};

  long nodes, msecs;

  public SearchGame () {
    super();
  }

  int solve()
  {
    int i,side;
    int x,work,score;
    long poscnt;
  
    nodes = 0L;
    msecs = 1L;
    side = (plycnt+1) & 1;
    for (i = 0; ++i <= 7 ; )
      if (height[i] <= 6) {
        if (wins(i, height[i], 1<<side) || colthr[columns[i]] == (1<<side))
          return (side!=0  ? WIN : LOSE) << 5;	// all score, no work:)
      }
    if ((x = transpose()) != ABSENT) {
      if ((x & 32) == 0)   // exact score
        return x;
    }
    JGFInstrumentor.startTimer("Section3:AlphaBetaSearch:Run"); 
    score = ab(LOSE,WIN);
    poscnt = posed;
    for (work=1; (poscnt>>=1) != 0; work++) ; //work = log of #positions stored 
    JGFInstrumentor.stopTimer("Section3:AlphaBetaSearch:Run"); 
    return score << 5 | work;
  }

  int ab(int alpha, int beta)
  {
    int besti,i,j,h,k,l,val,score;
    int x,v,work;
    int nav, av[] = new int[8];
    long poscnt;
    int side, otherside;
  
    nodes++;
    if (plycnt == 41)
      return DRAW;
    side = (otherside = plycnt & 1) ^ 1;
    for (i = nav = 0; ++i <= 7;) {
      if ((h = height[i]) <= 6) {
        if (wins(i,h,3) || colthr[columns[i]] != 0) {
          if (h+1 <= 6 && wins(i,h+1,1<<otherside))
            return LOSE;		// for 'o'
          av[0] = i;		// forget other moves
          while (++i <= 7)
            if ((h = height[i]) <= 6 &&
                (wins(i,h,3) || colthr[columns[i]] != 0))
              return LOSE;
          nav = 1;
          break;
        }
        if (!(h+1 <= 6 && wins(i,h+1,1<<otherside)))
          av[nav++] = i;
      }
    }
    if (nav == 0)
      return LOSE;
    if (nav == 1) {
      makemove(av[0]);
      score = -ab(-beta,-alpha);
      backmove();
      return score;
    }
    if ((x = transpose()) != ABSENT) {
      score = x >> 5;
      if (score == DRAWLOSE) {
        if ((beta = DRAW) <= alpha)
          return score;
      } else if (score == DRAWWIN) {
        if ((alpha = DRAW) >= beta)
          return score;
      } else return score; // exact score
    }
    poscnt = posed;
    l = besti = 0;	// initialize arbitrarily for silly javac
    score = Integer.MIN_VALUE;	// try to get the best bound if score > beta
    for (i = 0; i < nav; i++) {
      for (j = i, val = Integer.MIN_VALUE; j < nav; j++) {
        k = av[j];
        v = history[side][height[k]<<3|k];
        if (v > val) {
          val = v; l = j;
        }
      }
      j = av[l];
      if (i != l) {
        av[l] = av[i]; av[i] = j;
      }
      makemove(j);
      val = -ab(-beta,-alpha);
      backmove();
      if (val > score) {
        besti = i;
        if ((score = val) > alpha && (alpha = val) >= beta) {
          if (score == DRAW && i < nav-1)
            score = DRAWWIN;
          break;
        }
      }
    }
    if (besti > 0) {
      for (i = 0; i < besti; i++)
        history[side][height[av[i]]<<3|av[i]]--;	// punish bad historiess
      history[side][height[av[besti]]<<3|av[besti]] += besti;
    }
    poscnt = posed - poscnt;
    for (work=1; (poscnt>>=1) != 0; work++) ;	// work=log #positions stored
    if (x != ABSENT) {
      if (score == -(x>>5))	// combine < and >
        score = DRAW;
      transrestore(score, work);
    } else transtore(score, work);
    // if (plycnt == REPORTPLY) {
    //  System.out.println(toString() + "##-<=>+#".charAt(4+score) + work);
    //}
    return score;
  }

}
