/*****

$Author: leo $
$Date: 1995/10/17 13:33:59 $
$Log: scannerdriver.c,v $
Revision 1.1.1.1  1995/10/17 13:33:59  leo
Initial frontend source tree.

Revision 0.1  1995/06/23 12:23:38  joachim
initial revision derived from arnauds frontend

$Revision: 1.1.1.1 $
$Source: /home/rama/timber/CVS/compiler/frontends/toyfront/src/scannerdriver.c,v $ 

*****/

# include <stdio.h>
# include "Positions.h"
# include "boos_scan.h"

# define Debug

main ()
{
   int Token, Count = 0;
   char Word [256];

   printf("Open Scanner");
   boos_scan_BeginScanner ();
   printf("Start scanning");
   do {
      Token = boos_scan_GetToken ();
      Count ++;
# ifdef Debug
      if (Token != boos_scan_EofToken)
	  (void) boos_scan_GetWord (Word);
      else
	  Word [0] = '\0';
      WritePosition (stdout, boos_scan_Attribute.Position);
      (void) printf ("%5d %s", Token, Word);
      switch (Token) {
	case (1):
	  GetString(boos_scan_Attribute.Identifier.Ident,Word);
	  printf(" attribute Identifier: %s\n",Word);
	  break;
	case (2):
	  printf(" attribute NaturalConst: %d\n",
		 boos_scan_Attribute.NaturalConst.Value);
	  break;
	case (3):
	  printf(" attribute IntegerConst: %d\n",
		 boos_scan_Attribute.IntegerConst.Value);
	  break;
	case (4):
	  printf(" attribute RealNumber: %f\n",
		 boos_scan_Attribute.RealConst.Value);
	  break;
	case (5):
	  StGetString(boos_scan_Attribute.StringLiteral.StringValue,Word);
	  printf(" attribute StringLiteral: %s\n", Word);
	  break;
	default:
	  printf("\n");
      }
# endif
  } while (Token != boos_scan_EofToken);
   boos_scan_CloseScanner ();
   (void) printf ("%d\n", Count);
   return 0;
}
