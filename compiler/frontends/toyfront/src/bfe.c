/*****

$Author: leo $
$Date: 1995/10/17 13:33:59 $
$Log: bfe.c,v $
Revision 1.1.1.1  1995/10/17 13:33:59  leo
Initial frontend source tree.

Revision 0.1  1995/06/23 12:22:10  joachim
initial revision derived from arnauds frontend

$Revision: 1.1.1.1 $
$Source: /home/rama/timber/CVS/compiler/frontends/toyfront/src/bfe.c,v $ 

*****/



# include "boos_pars.h"
# include "Tree.h"
# include "BuildScopes.h"

main 
# if defined __STDC__ | defined __cplusplus
(int argc, char * argv[])
# else
(argc, argv)
int  argc;
char * argv[];
# endif
{
  FILE *f;
  int ErrorCount;

  if (argc != 2) {
    printf("usage: parserdriver <file>.boo\n");
    exit(1);
  };

  if (strcmp(argv[1]+strlen(argv[1])-4,".boo") != 0) {
    printf("usage: parserdriver <file>.boo\n");
    exit(1);
  };

  if ((f = fopen(argv[1],"r")) == NULL) {
    perror(argv[1]);
    exit(1);
  };


  BeginTree();

  boos_scan_BeginFile(argv[1]);

  printf ("Parsing %s\n",argv[1]);

  if ((ErrorCount = boos_pars()) != 0) {
    printf("%d syntax errors found while parsing file %s\n", 
      ErrorCount, argv[1]);
  } else {
    
    BeginBuildScopes;
    BuildScopes(TreeRoot);
    CloseBuildScopes; 
    WriteTree(stdout, TreeRoot);

  };

  printf ("Finished parsing %s\n",argv[1]);

  
}


