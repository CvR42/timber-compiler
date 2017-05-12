/*****

$Author: leo $
$Date: 1995/10/17 13:33:59 $
$Log: MagicNumbers.h,v $
Revision 1.1.1.1  1995/10/17 13:33:59  leo
Initial frontend source tree.

Revision 0.2  1995/07/11 09:35:38  joachim
removed some garbage characters

Revision 0.1  1995/06/23 12:57:00  joachim
initial revision

$Revision: 1.1.1.1 $
$Source: /home/rama/timber/CVS/compiler/frontends/toyfront/src/MagicNumbers.h,v $ 

*****/

# ifndef MagicNumbers

# define MagicNumbers 

/* Obviously this is already defined elsewhere (?)

typedef short bool;             /* the data type bool                   

# define false 0
# define true  1
*/

# define MaxCharsPerString 2048
                                /* defines the maximum length of a string*/
                                /* the scanner is able to handle         */
                                /* Note: the generated code does *not*   */
                                /* check this limit !                    */

                                /* A scope in Booster consists of four   */
                                /* namespaces T (types), D (variables and*/
                                /* constants), and P (procedures) and    */
                                /* F (functions). The following constants*/
                                /* define the necessary labels           */

# define namespaceD 0           
# define namespaceT 1
# define namespaceP 2
# define namespaceF 3 

# define ExternFun 0            /* distinguishes an externally defined   */    
# define BoosterFun 1           /* function from a Booster function      */

# define ProgModule 0           /* distinguishes a program module from   */
# define ImplModule 1           /* from an implementation module         */

# define FlowTypeIn 3           /* distinguishe the different kinds of   */
# define FlowTypeOut 4          /* formal parameters                     */
# define FlowTypeInOut 5

# define NoOp 6                 /* codes to represent operators in the   */
                                /* abstract syntax tree of expressions   */
# define Greater 7
# define GreaterEq 8
# define UnEqual 9
# define Equal 10
# define SmallerEq 11
# define Smaller 12 
# define SetOr 13
# define SetAnd 14
# define Or 15
# define AddPlus 16
# define AddMinus 17
# define And 18
# define Dots 19
# define Div 20 	
# define Mod 21
# define Times 22
# define Divide 23
# define SetNot 24
# define Not 25
# define UnPlus 26
# define UnMinus 27

# endif
