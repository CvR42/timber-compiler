/*
  Lot of parsing of pragmas.
  For now the parsing of expressions and MPexpressions contains a lot of
  similar code. This shoul;d be unified and merged.
  */

#include <assert.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <tmc.h>

#include "defs.h"
#include "tmadmin.h"
#include "vnusctl.h"
#include "global.h"
#include "error.h"
#include "service.h"
#include "parsepragmas.h"
//#define VERBOSE_ON

static Arc parse_Arc(PragmaExpression_list l);
static MPexpression parse_MPexpression(PragmaExpression x);

static bool isIntNumberPragmaExpression(PragmaExpression pe){
    if (pe->tag != TAGNumberPragmaExpression)
	return false;
    NumberPragmaExpression index = to_NumberPragmaExpression(pe);
    return (ceil(index->v) == floor(index->v));
}

static bool isDoubleNumberPragmaExpression(PragmaExpression pe){
    return (pe->tag == TAGNumberPragmaExpression);
}

static bool isNamePragmaExpression(PragmaExpression pe, const char *s){
    if (pe->tag != TAGNamePragmaExpression)
	return false;
    assert (s != NULL);
    return (to_NamePragmaExpression(pe)->name->sym == add_tmsymbol(s));
}

/*
  Parse a list of processor types, each must be:
  <name> <string>
  */
static void parse_ProcessorTypes(global_context gc, Pragma p)
{
    assert (p->tag == TAGValuePragma);
    ValuePragma vp = to_ValuePragma(p);
    assert (vp->x->tag == TAGListPragmaExpression);
    ListPragmaExpression outer_l= to_ListPragmaExpression(vp->x);
    for (unsigned i=0; i<outer_l->l->sz; i++)
    {
	assert (outer_l->l->arr[i]->tag == TAGListPragmaExpression);
	ListPragmaExpression one_type = to_ListPragmaExpression(outer_l->l->arr[i]);
	assert (one_type->l->sz == 2);
	assert (one_type->l->arr[0]->tag == TAGNamePragmaExpression);
	assert (one_type->l->arr[1]->tag == TAGStringPragmaExpression);
	NamePragmaExpression npe = to_NamePragmaExpression(one_type->l->arr[0]);
	StringPragmaExpression spe = to_StringPragmaExpression(one_type->l->arr[1]);
	SimpleProcType spr = new_SimpleProcType(
	    rdup_origsymbol(npe->name),
	    rdup_tmstring(spe->s));
	gc->processorTypes = append_ProcessorType_list(gc->processorTypes, spr);
#ifdef VERBOSE_ON
	fprintf(stderr,"Parsed processor type: %s:%s\n",npe->name->sym->name,spe->s);
#endif
    }
}

/*
  Parse a list of processors, each must be:
  <typename> <name>
  <typename> <name> [<dimsize>+]

  The second one is actually stored as:
  <typename> ("at" <name> <dimsize>+)
  
  Dimensions should be integer, but were parsed as double
  -> check that they don't contain any character from "+-.,Ee" using strcspn
  */
static void parse_ProcessorDecls(global_context gc, Pragma p)
{
    assert (p->tag == TAGValuePragma);
    ValuePragma vp = to_ValuePragma(p);
    assert (vp->x->tag == TAGListPragmaExpression);
    ListPragmaExpression outer_l= to_ListPragmaExpression(vp->x);
    for (unsigned i=0; i<outer_l->l->sz; i++)
    {
	assert (outer_l->l->arr[i]->tag == TAGListPragmaExpression);
	ListPragmaExpression one_decl = to_ListPragmaExpression(outer_l->l->arr[i]);
	assert (one_decl->l->sz == 2);
	assert (one_decl->l->arr[0]->tag == TAGNamePragmaExpression);
	NamePragmaExpression typeName = to_NamePragmaExpression(one_decl->l->arr[0]);
	if (one_decl->l->arr[1]->tag == TAGNamePragmaExpression)
	{
	    NamePragmaExpression declName = to_NamePragmaExpression(one_decl->l->arr[1]);
	    SimpleProcDecl npd = new_SimpleProcDecl(
		rdup_origsymbol(declName->name),
		rdup_origsymbol(typeName->name));
	    gc->processors = append_ProcessorDecl_list(gc->processors, npd);
#ifdef VERBOSE_ON
	    fprintf(stderr,"Parsed processor decl: %s:%s\n",declName->name->sym->name,typeName->name->sym->name);
#endif
	}
	else if (one_decl->l->arr[1]->tag == TAGListPragmaExpression)
	{
	    ListPragmaExpression decl_type = to_ListPragmaExpression(one_decl->l->arr[1]);
	    assert (decl_type->l->sz > 2);
	    assert (decl_type->l->arr[0]->tag == TAGNamePragmaExpression);
	    assert (isNamePragmaExpression(decl_type->l->arr[0], "at"));
	    assert (decl_type->l->arr[1]->tag == TAGNamePragmaExpression);
	    NamePragmaExpression declName = to_NamePragmaExpression(decl_type->l->arr[1]);
	    for (unsigned j=2; j<decl_type->l->sz; j++)
	    {
		assert(isIntNumberPragmaExpression(decl_type->l->arr[j]));
	    }
	    ArrayProcDecl npd = new_ArrayProcDecl(
		rdup_origsymbol(declName->name),
		rdup_origsymbol(typeName->name),
		new_vnus_int_list());
	    for (unsigned j=2; j<decl_type->l->sz; j++)
	    {
		NumberPragmaExpression index = to_NumberPragmaExpression(decl_type->l->arr[j]);
		npd->sizes = append_vnus_int_list(npd->sizes, (int)floor(index->v));
	    }
	    gc->processors = append_ProcessorDecl_list(gc->processors, npd);
#ifdef VERBOSE_ON
	    fprintf(stderr,"Parsed processor decl: %s:%s[",declName->name->sym->name,typeName->name->sym->name);
	    for (unsigned j=2; j<decl_type->l->sz; j++)
	    {
		NumberPragmaExpression index = to_NumberPragmaExpression(decl_type->l->arr[j]);
		fprintf(stderr,"%s,",index->v);
	    }
	    fprintf(stderr,"]\n");
#endif
	}
	else
	{
	    internal_error("type of processor can be either single or array");
	}
    }
}

void parseOldProcessorDecls(global_context gc, Pragma p)
{
    if (p->tag != TAGValuePragma)
	return;
    PragmaExpression x = to_ValuePragma(p)->x;
    if( x->tag != TAGStringPragmaExpression ){
        return;
    }
    tmstring str = to_StringPragmaExpression(x)->s;
    ArrayProcDecl apd = new_ArrayProcDecl(
	add_origsymbol("P"),
	add_origsymbol("generic"),
	new_vnus_int_list());
    for(;;){
	while( *str != '\0' && !isdigit( *str ) ){
	    str++;
	}
	if( *str == '\0' ){
	    break;
	}
	unsigned int nproc = 0;
	while( isdigit( *str ) ){
	    nproc = nproc*10+(*str - '0');
	    str++;
	}
	apd->sizes = append_vnus_int_list(
	    apd->sizes,
	    nproc);
    }
    gc->processors = append_ProcessorDecl_list(
	gc->processors,
	apd);
}

void generate_dummyProcessorDecls(global_context gc)
{
  assert(gc->processors->sz == 0);
  ArrayProcDecl apd = new_ArrayProcDecl(
					add_origsymbol("P"),
					add_origsymbol("generic"),
					new_vnus_int_list());
  apd->sizes = append_vnus_int_list(
				    apd->sizes,
				    1);
  gc->processors = append_ProcessorDecl_list(
					      gc->processors,
					      apd);
}

void generate_ProcessorDecls(global_context gc)
{
    ArrayProcDecl apd = get_processor_decl(gc);
    assert(apd->sizes->sz > 0);
    ListPragmaExpression arr_l = new_ListPragmaExpression(
	new_PragmaExpression_list());
    arr_l->l = append_PragmaExpression_list(
	arr_l->l,
	new_NamePragmaExpression(add_origsymbol("at")));
    arr_l->l = append_PragmaExpression_list(
	arr_l->l,
	new_NamePragmaExpression(rdup_origsymbol(apd->name)));
    for (unsigned i=0; i<apd->sizes->sz; i++)
    {
	arr_l->l = append_PragmaExpression_list(
	    arr_l->l,
	    new_NumberPragmaExpression(apd->sizes->arr[i]));
    }
    ListPragmaExpression decl_l = new_ListPragmaExpression(
	new_PragmaExpression_list());
    decl_l->l = append_PragmaExpression_list(
	decl_l->l,
	new_NamePragmaExpression(rdup_origsymbol(apd->type)));
    decl_l->l = append_PragmaExpression_list(
	decl_l->l,
	arr_l);
    ListPragmaExpression outer_l = new_ListPragmaExpression(
	new_PragmaExpression_list());
    outer_l->l = append_PragmaExpression_list(
	outer_l->l,
	decl_l);
    ValuePragma vp = new_ValuePragma(
	add_origsymbol("Processors"),
	outer_l);
    gc->prog->pragmas = append_Pragma_list(
	gc->prog->pragmas,
	vp);
}

void generate_OldProcessorDecls(global_context gc)
{
    ArrayProcDecl apd = get_processor_decl(gc);
    assert(apd->sizes->sz > 0);
    char s[256];
    sprintf( s, "%d", apd->sizes->arr[0]);
    for (unsigned i=1; i<apd->sizes->sz; i++){
	sprintf( &s[strlen(s)],",%d", apd->sizes->arr[i]);
    }
    assert( strlen(s) < 256 );
    ValuePragma vp = new_ValuePragma(
	add_origsymbol("processors"),
	new_StringPragmaExpression(new_tmstring(s)));
    gc->prog->pragmas = append_Pragma_list( gc->prog->pragmas, vp );
}

void generate_ProcessorTypes(global_context gc)
{
    assert(gc->processorTypes->sz == 0);
    SimpleProcType spr = new_SimpleProcType(
	add_origsymbol("generic"),
	new_tmstring("Generic"));
    gc->processorTypes = append_ProcessorType_list(gc->processorTypes, spr);

    ListPragmaExpression one_type = new_ListPragmaExpression(
	new_PragmaExpression_list());
    one_type->l = append_PragmaExpression_list(
	one_type->l,
	new_NamePragmaExpression(
	    rdup_origsymbol(spr->name)));
    one_type->l = append_PragmaExpression_list(
	one_type->l,
	new_StringPragmaExpression(
	    rdup_tmstring(spr->type)));
    ListPragmaExpression outer_l = new_ListPragmaExpression(
	new_PragmaExpression_list());
    outer_l->l = append_PragmaExpression_list(
	outer_l->l,
	one_type);
    ValuePragma vp = new_ValuePragma(
	add_origsymbol("ProcessorType"),
	outer_l);
    gc->prog->pragmas = append_Pragma_list(
	gc->prog->pragmas,
	vp);
}

void calculate_numberOfProcessors(global_context gc)
{
	/*
	  Semantic checks on processors array.
	  Need some extra checking like:
	  - are all used types also declared?
	  
	  For now only one processor array
	  */
    ArrayProcDecl apd = get_processor_decl(gc);
    gc->numberOfProcessors = 1;
    for (unsigned i=0; i<apd->sizes->sz; i++)
      {
	assert(apd->sizes->arr[i] != 0);
	gc->numberOfProcessors *= apd->sizes->arr[i];
      }
}

/*
  Parse both oldstyle and newstyle processor pragmas.
  Newstyle pragmas override oldstyle pragmas.
  Remove (possible inconsistent) oldstyle pragmas.
  Generate consistent oldstyle and newstyle pragams if not present.
  Also check for enableSymbolicNumberOfProcessors flag. In that case
  generate a pragma(if not already present), and adjust semantic checking.
  */
void parse_global_pragmas(global_context gc)
{
    if (gc->prog->pragmas == Pragma_listNIL)
	gc->prog->pragmas = new_Pragma_list();
    gc->processorTypes = new_ProcessorType_list();
    gc->processors = new_ProcessorDecl_list();
    
    /*
      Before parsing processor stuff, first check for symbolic
      numberOfProcessors.
      Generate pragma if flag is set, and set flag if pragma is found.
    */
    bool eSNOP = pragma_list_contains_tmsymbol(gc->prog->pragmas,add_tmsymbol("enableSymbolicNumberOfProcessors"));
    if (!eSNOP && enableSymbolicNumberOfProcessors)
      {
	gc->prog->pragmas = append_Pragma_list
	  (
	   gc->prog->pragmas,
	   new_FlagPragma(add_origsymbol("enableSymbolicNumberOfProcessors")));
      }
    if (eSNOP)
      {
	enableSymbolicNumberOfProcessors = true;
      }

	/*
	  First parse newstyle processor and processor type decls.
	  */
    for (unsigned i=0; i<gc->prog->pragmas->sz; i++)
    {
	if (gc->prog->pragmas->arr[i]->name->sym == add_tmsymbol("ProcessorType"))
	{
	    parse_ProcessorTypes(gc, gc->prog->pragmas->arr[i]);
	}
	else if (gc->prog->pragmas->arr[i]->name->sym == add_tmsymbol("Processors"))
	{
	    parse_ProcessorDecls(gc, gc->prog->pragmas->arr[i]);
	}
    }

	/*
	  If no processor type, make a default one.
	  */
    if (gc->processorTypes->sz == 0)
    {
      generate_ProcessorTypes(gc);
    }

	/*
	  If no newstyle processor decl, try an oldstyle one and generate a newstyle one.
	  */
    if (gc->processors->sz == 0)
    {
	int pos = pragma_list_find_index_tmsymbol(gc->prog->pragmas, add_tmsymbol("processors"));
	if (pos != -1)
	{
	    parseOldProcessorDecls(gc, gc->prog->pragmas->arr[pos]);
	    assert (pragma_list_find_index_tmsymbol(gc->prog->pragmas, add_tmsymbol("Processors")) == -1);
	    generate_ProcessorDecls(gc);
	}
    }
    /*
      If we want symbolic instead of constant(fixed) numberOfProcessors, it might
      be possible that at this point there is no processor array definition.
      Just to be on the safe side, make it a 1-dim array of size 1 if not present.
    */
    if (enableSymbolicNumberOfProcessors && gc->processors->sz == 0)
      {
	generate_dummyProcessorDecls(gc);
	generate_ProcessorDecls(gc);
      }
	/*
	  At this point there should be a definition of a processor array.
	  */
    assert (gc->processors->sz != 0);
    /*
      If we are doing symbolic stuff, only a 1-dim array is allowed.
    */
    assert (!enableSymbolicNumberOfProcessors || gc->processors->sz == 1);
    
	/*
	  Just to sure sure for backwards compatibility, make sure that
	  an oldstyle processor decl is present which is consistent with
	  parsed processor array.
	  This means zapping any 'overridden' versions.
	  */
    {
	int pos = pragma_list_find_index_tmsymbol(gc->prog->pragmas, add_tmsymbol("processors"));
	if (pos != -1)
	{
	    gc->prog->pragmas = delete_Pragma_list(gc->prog->pragmas, pos);
	}
	generate_OldProcessorDecls(gc);
    }

    /*
      The following can even be done in the symbolic case.
      The only thing that is important is that the array is 1-dim.
    */
    calculate_numberOfProcessors(gc);
    assert (gc->numberOfProcessors > 0);
}

static MPExprBinop parse_MPExprBinop(PragmaExpression_list l, BINOP b)
{
    assert (l->sz >2);
    MPExprBinop res = new_MPExprBinop(
	parse_MPexpression(l->arr[1]),
	b,
	parse_MPexpression(l->arr[2]));
    for (unsigned i=3; i<l->sz; i++)
    {
	res = new_MPExprBinop(
	    res,
	    b,
	    parse_MPexpression(l->arr[i]));
    }
    return res;
}

static ExprBinop parse_ExprBinop(PragmaExpression_list l, BINOP b)
{
    assert (l->sz >2);
    ExprBinop res = new_ExprBinop(
	parse_expression(l->arr[1]),
	b,
	parse_expression(l->arr[2]));
    for (unsigned i=3; i<l->sz; i++)
    {
	res = new_ExprBinop(
	    res,
	    b,
	    parse_expression(l->arr[i]));
    }
    return res;
}

static MPexpression parse_MPexpression(PragmaExpression x){
    if (x->tag == TAGExternalNamePragmaExpression)
    {
	return new_MPExprExternalName(
	    rdup_origsymbol(to_ExternalNamePragmaExpression(x)->name));
    }
    if (x->tag == TAGListPragmaExpression)
    {
	PragmaExpression_list l = to_ListPragmaExpression(x)->l;
	assert (l != PragmaExpression_listNIL);
	assert (l->sz>0);
	if (isNamePragmaExpression(l->arr[0],"at"))
	{
	    assert (l->sz>2);
	    MPExprSelection shape = new_MPExprSelection(
		parse_MPexpression(l->arr[1]),
		new_MPexpression_list());
	    for (unsigned i=2; i<l->sz; i++)
	    {
		shape->indices = append_MPexpression_list(
		    shape->indices,
		    parse_MPexpression(l->arr[i]));
	    }
	    return shape;
	}
	if (isNamePragmaExpression(l->arr[0], "sum"))
	    return parse_MPExprBinop(l, BINOP_PLUS);
	if (isNamePragmaExpression(l->arr[0], "prod"))
	    return parse_MPExprBinop(l, BINOP_TIMES);
	if (isNamePragmaExpression(l->arr[0], "subtract"))
	    return parse_MPExprBinop(l, BINOP_MINUS);
	if (isNamePragmaExpression(l->arr[0], "div"))
	    return parse_MPExprBinop(l, BINOP_DIVIDE);
	if (isNamePragmaExpression(l->arr[0], "mod"))
	    return parse_MPExprBinop(l, BINOP_MOD);
	if (isNamePragmaExpression(l->arr[0], "eq"))
	    return parse_MPExprBinop(l, BINOP_EQUAL);
	if (isNamePragmaExpression(l->arr[0], "ne"))
	    return parse_MPExprBinop(l, BINOP_NOTEQUAL);
	if (isNamePragmaExpression(l->arr[0], "le"))
	    return parse_MPExprBinop(l, BINOP_LESS);
	if (isNamePragmaExpression(l->arr[0], "lt"))
	    return parse_MPExprBinop(l, BINOP_LESSEQUAL);
	if (isNamePragmaExpression(l->arr[0], "ge"))
	    return parse_MPExprBinop(l, BINOP_GREATER);
	if (isNamePragmaExpression(l->arr[0], "gt"))
	    return parse_MPExprBinop(l, BINOP_GREATEREQUAL);
	if (l->sz == 1)
	    return parse_MPexpression(l->arr[0]);
	internal_error("PaPr:bummer: unexpected list");
    }
    if (isIntNumberPragmaExpression(x))
    {
	return new_MPExprInt((int)floor(to_NumberPragmaExpression(x)->v));
    }
    if (isDoubleNumberPragmaExpression(x))
    {
	return new_MPExprDouble(to_NumberPragmaExpression(x)->v);
    }
    if (x->tag == TAGBooleanPragmaExpression)
    {
	return new_MPExprBoolean(rdup_vnus_boolean(to_BooleanPragmaExpression(x)->b));
    }
    if (x->tag == TAGNamePragmaExpression)
    {
	return new_MPExprName(
	    rdup_origsymbol(to_NamePragmaExpression(x)->name));
    }
    if (x->tag == TAGStringPragmaExpression)
    {
	internal_error("PaPr:bummer: unexpected string");
    }
    internal_error("PaPr:parse_MPexpression: unexpected pragma");
    return MPexpressionNIL;
}

/*
  Parse expressions.
  For now only integer expressions.
  Everything calling this function depends on this, so if later on doubles etc.
  are added additional semantic checking will be necessary.
  Note: external variables are not checked for being of integer type.
  */
expression parse_expression(PragmaExpression x){
    if (x->tag == TAGExternalNamePragmaExpression)
    {
	return new_ExprName(
	    rdup_origsymbol(to_ExternalNamePragmaExpression(x)->name));
    }
    if (x->tag == TAGListPragmaExpression)
    {
	PragmaExpression_list l = to_ListPragmaExpression(x)->l;
	assert (l != PragmaExpression_listNIL);
	assert (l->sz>0);
	if (isNamePragmaExpression(l->arr[0],"at"))
	{
	    assert (l->sz>2);
	    ExprSelection shape = new_ExprSelection(
		parse_expression(l->arr[1]),
		new_expression_list());
	    for (unsigned i=2; i<l->sz; i++)
	    {
		shape->indices = append_expression_list(
		    shape->indices,
		    parse_expression(l->arr[i]));
	    }
	    return shape;
	}
	if (isNamePragmaExpression(l->arr[0], "sum"))
	    return parse_ExprBinop(l, BINOP_PLUS);
	if (isNamePragmaExpression(l->arr[0], "prod"))
	    return parse_ExprBinop(l, BINOP_TIMES);
	if (isNamePragmaExpression(l->arr[0], "subtract"))
	    return parse_ExprBinop(l, BINOP_MINUS);
	if (isNamePragmaExpression(l->arr[0], "div"))
	    return parse_ExprBinop(l, BINOP_DIVIDE);
	if (isNamePragmaExpression(l->arr[0], "mod"))
	    return parse_ExprBinop(l, BINOP_MOD);
	if (l->sz == 1)
	    return parse_expression(l->arr[0]);
	internal_error("PaPr:bummer: unexpected list");
    }
    if (isIntNumberPragmaExpression(x))
    {
	return new_ExprInt((int)floor(to_NumberPragmaExpression(x)->v));
    }
    if (isDoubleNumberPragmaExpression(x))
    {
	internal_error("PaPr:bummer: unexpected double");
//	return new_ExprDouble(rdup_tmstring(to_NumberPragmaExpression(x)->v));
    }
    if (x->tag == TAGBooleanPragmaExpression)
    {
	internal_error("PaPr:bummer: unexpected boolean");
//	return new_ExprBoolean(rdup_vnus_boolean(to_BooleanPragmaExpression(x)->b));
    }
    if (x->tag == TAGNamePragmaExpression)
    {
	return new_ExprName(
	    rdup_origsymbol(to_NamePragmaExpression(x)->name));
    }
    if (x->tag == TAGStringPragmaExpression)
    {
	internal_error("PaPr:bummer: unexpected string");
    }
    internal_error("PaPr:parse_expression: unexpected pragma");
    return expressionNIL;
}

static MPexpression_list parse_MPexpression_list(PragmaExpression x)
{
    MPexpression_list res = new_MPexpression_list();
    switch (x->tag)
    {
	case TAGListPragmaExpression:
	{
	    PragmaExpression_list l = to_ListPragmaExpression(x)->l;
	    if (l != PragmaExpression_listNIL)
		for (unsigned i=0; i<l->sz; i++)
		    res = append_MPexpression_list(
			res,
			parse_MPexpression(l->arr[i]));
	    break;
	}
	default:
	    res = append_MPexpression_list(
		res,
		parse_MPexpression(x));
	    break;
    }
    return res;
}

static MPoptexpression create_MPoptexpression(MPexpression x)
{
    if (x==MPexpressionNIL)
    {
	return new_MPOptExprNone();
    }
    else
    {
	return new_MPOptExpr(x);
    }
}

TaskId parse_task_pragma(Pragma p)
{
    origsymbol name = origsymbolNIL;
    MPexpression cost = MPexpressionNIL;

    assert(p->tag == TAGValuePragma);
    ValuePragma vp = to_ValuePragma(p);
    assert (vp->x->tag == TAGListPragmaExpression);
    PragmaExpression_list l = to_ListPragmaExpression(vp->x)->l;

    assert (l->sz == 1 || l->sz == 2);
    assert (l->arr[0]->tag == TAGNamePragmaExpression);
    name = rdup_origsymbol(to_NamePragmaExpression(l->arr[0])->name);
    if (l->sz == 2)
	cost = parse_MPexpression(l->arr[1]);
    fprintf(stderr,"PaPr: Parsed task with id:%s\n",name->sym->name);
    TaskId res = new_TaskId(
	name,
	create_MPoptexpression(cost));
    return res;
}

int parse_taskid_pragma(Pragma p)
{
    assert(p->tag == TAGValuePragma);
    ValuePragma vp = to_ValuePragma(p);
    assert (isIntNumberPragmaExpression(vp->x));
    int res = (int)floor(to_NumberPragmaExpression(vp->x)->v);
    fprintf(stderr,"PaPr: Parsed taskid with id:%d\n",res);
    return res;
}

static BasicArc parse_BasicArc(PragmaExpression_list l)
{
    origsymbol name = origsymbolNIL;
    MPexpression_list indices = MPexpression_listNIL;
    MPexpression cost = MPexpressionNIL;
    MPexpression data = MPexpressionNIL;
    
    assert (l->sz > 1 && l->sz < 5);
    assert (l->arr[0]->tag == TAGNamePragmaExpression);

    name = rdup_origsymbol(to_NamePragmaExpression(l->arr[0])->name);
    indices = parse_MPexpression_list(l->arr[1]);
    if (l->sz > 2)
	cost = parse_MPexpression(l->arr[2]);
    if (l->sz == 4)
	data = parse_MPexpression(l->arr[3]);

    BasicArc res = new_BasicArc(
	name,
	indices,
	create_MPoptexpression(cost),
	create_MPoptexpression(data));
    return res;
}

static ConditionalArc parse_ConditionalArc(PragmaExpression_list l)
{
    assert (l->sz == 4);
    assert (isNamePragmaExpression(l->arr[0], "if"));
    assert (l->arr[2]->tag == TAGListPragmaExpression);
    assert (l->arr[3]->tag == TAGListPragmaExpression);
    return new_ConditionalArc(
	parse_MPexpression(l->arr[1]),
	parse_Arc(to_ListPragmaExpression(l->arr[2])->l),
	parse_Arc(to_ListPragmaExpression(l->arr[3])->l));
}

static IterationArc parse_IterationArc(PragmaExpression_list l)
{
    assert (l->sz == 4);
    assert (isNamePragmaExpression(l->arr[0], "for"));
    assert (l->arr[1]->tag == TAGNamePragmaExpression);
    assert (l->arr[3]->tag == TAGListPragmaExpression);
    return new_IterationArc(
	rdup_origsymbol(to_NamePragmaExpression(l->arr[1])->name),
	parse_MPexpression(l->arr[2]),
	parse_Arc(to_ListPragmaExpression(l->arr[3])->l));
}

static Arc parse_Arc(PragmaExpression_list l)
{
    Arc res = ArcNIL;
    assert (l->sz >0);
    if (isNamePragmaExpression(l->arr[0], "for"))
	res = parse_IterationArc(l);
    else if (isNamePragmaExpression(l->arr[0], "if"))
	res = parse_ConditionalArc(l);
    else
	res = parse_BasicArc(l);
    return res;
}

static Arc parse_arc_pragma(Pragma p)
{
    Arc res = ArcNIL;
    assert (p->tag == TAGValuePragma);
    ValuePragma vp = to_ValuePragma(p);
    assert (vp->x->tag == TAGListPragmaExpression);
    PragmaExpression_list l = to_ListPragmaExpression(vp->x)->l;

    res = parse_Arc(l);
    return res;
}

Arc_list parse_arc_pragmas(Pragma_list pl, tmsymbol key)
{
    Arc_list res = Arc_listNIL;
    if (pl == Pragma_listNIL || key == tmsymbolNIL)
	return res;
    for (unsigned i=0; i<pl->sz; i++)
    {
	if (pl->arr[i]->name->sym == key)
	{
	    if (res == Arc_listNIL)
		res = new_Arc_list();
	    res = append_Arc_list(
		res,
		parse_arc_pragma(pl->arr[i]));
	}
    }
    return res;
}

/*
  parses
  PlacementFunction:
      ( 'block'  index-PragmaExpression blocksize-PragmaExpression )
      ( 'cyclic' index-PragmaExpression extent-PragmaExpression )
      ( 'blockcyclic' index-PragmaExpression blocksize-PragmaExpression extent-PragmaExpression )
      _all
  or:
      simpel expression
  */
static DistrExpr parse_distribution(PragmaExpression x)
{
    if (x->tag == TAGListPragmaExpression)
    {
	PragmaExpression_list p_l = to_ListPragmaExpression(x)->l;

	if (p_l->sz == 1)
	{
	    return parse_distribution(p_l->arr[0]);
	}
	if (isNamePragmaExpression(p_l->arr[0], "block"))
	{
	    switch (p_l->sz)
	    {
		case 2:
		    internal_error("P2A: block without size parameter not yet implemented");
		    return new_NewDistBlock(parse_expression(p_l->arr[1]), 
					    new_ExprInt(0));
		    break;
		case 3:
		    return new_NewDistBlock(parse_expression(p_l->arr[1]), 
					    parse_expression(p_l->arr[2]));
		    break;
		default:
		    internal_error("P2A: block: expected 1 or 2 parameters");
		    break;
	    }
	}
	else if (isNamePragmaExpression(p_l->arr[0], "cyclic"))
	{
	    switch (p_l->sz)
	    {
		case 2:
		    return new_NewDistCyclic(parse_expression(p_l->arr[1]), new_ExprInt(0));
		    break;
		case 3:
		    return new_NewDistCyclic(parse_expression(p_l->arr[1]), 
					     parse_expression(p_l->arr[2]));
		    break;
		default:
		    internal_error("P2A: cyclic: expected 1 or 2 parameters");
		    break;
	    }
	}
	else if (isNamePragmaExpression(p_l->arr[0], "blockcyclic"))
	{
	    switch (p_l->sz)
	    {
		case 3:
		    return new_NewDistBC(parse_expression(p_l->arr[1]), 
					 parse_expression(p_l->arr[2]), new_ExprInt(0));
		    break;
		case 4:
		    return new_NewDistBC(parse_expression(p_l->arr[1]), 
					 parse_expression(p_l->arr[2]), parse_expression(p_l->arr[3]));
		    break;
		default:
		    internal_error("P2A: blockcyclic: expected 2 or 3 parameters");
		    break;
	    }
	}
	else if (isNamePragmaExpression(p_l->arr[0], "local"))
	{
	    switch (p_l->sz)
	    {
		case 2:
		    return new_NewDistLocal(parse_expression(p_l->arr[1]));
		    break;
		default:
		    internal_error("P2A: local: expected 1 parameter");
		    break;
	    }
	}
    }
    else if (isNamePragmaExpression(x, "_all"))
    {
	return new_NewDistReplicated();
    }
    else if (isNamePragmaExpression(x, "_"))
    {
	return new_NewDistDontCare();
    }
	// Default case assume simple expression and loca distribution.
    return new_NewDistLocal(parse_expression(x));
}

/*
  Parse:
    NewOwnDontCare
    NewOwnReplicated
    NewOwnLocation
    */
static OwnerExpr parse_simple_owner(PragmaExpression x)
{
    if (isNamePragmaExpression(x,"_all"))
    {
	return new_NewOwnReplicated();
    }
    else if (isNamePragmaExpression(x,"_"))
    {
	return new_NewOwnDontCare();
    }
    else if (x->tag == TAGListPragmaExpression)
    {
	PragmaExpression_list p_l = to_ListPragmaExpression(x)->l;
	if (p_l->sz == 1)
	{
	    return parse_simple_owner(p_l->arr[0]);
	}
	else if (p_l->sz > 2 &&
		 isNamePragmaExpression(p_l->arr[0],"at") &&
		 p_l->arr[1]->tag == TAGNamePragmaExpression)
	{
	    DistrExpr_list distr_l = new_DistrExpr_list();
	    
	    for (unsigned int i = 2; i < p_l->sz; i++)
	    {
		distr_l = append_DistrExpr_list(distr_l, parse_distribution(p_l->arr[i]));
	    }
	    return new_NewOwnLocation(
		new_ExprName(
		    rdup_origsymbol(to_NamePragmaExpression(p_l->arr[1])->name)),
		distr_l);
	}
    }
    internal_error("P2A: _, _all or <proc>[...] expected");
    return OwnerExprNIL;
}

/*
  parses
  FormalParams:
      ( Formal-Identifier-seq )
  */
static origsymbol_list parse_formal_params(PragmaExpression x)
{
    assert (x != PragmaExpressionNIL && x->tag == TAGListPragmaExpression);
    PragmaExpression_list p_l = to_ListPragmaExpression(x)->l;
    origsymbol_list os_l = new_origsymbol_list();

    if (p_l != PragmaExpression_listNIL)
    {
	for (unsigned int i = 0; i < p_l->sz; i++)
	{
	    if (p_l->arr[i]->tag == TAGNamePragmaExpression)
	    {
		os_l = append_origsymbol_list(
		    os_l,
		    rdup_origsymbol(
			to_NamePragmaExpression(p_l->arr[i])->name));
	    }
	    else
	    {
		internal_error("P2A: Formal-Identifier expected");
	    }
	}
    }
    return os_l;
}

/*
  Parse:
    NewOwnMap
  or do parse_simple_owner
    */
static OwnerExpr parse_abstract_owner(PragmaExpression x)
{
    if (x->tag == TAGListPragmaExpression)
    {
	PragmaExpression_list p_l = to_ListPragmaExpression(x)->l;
	if (p_l->sz == 1)
	{
	    return parse_abstract_owner(p_l->arr[0]);
	}
	else if (isNamePragmaExpression(p_l->arr[0], "lambda"))
	{
	    if (p_l->sz == 3 && p_l->arr[1]->tag == TAGListPragmaExpression)
	    {
		origsymbol_list parm_l = parse_formal_params(p_l->arr[1]);
		OwnerExpr on_expr = parse_simple_owner(p_l->arr[2]);
		
		return new_NewOwnMap(parm_l, on_expr);
	    }
	    else
	    {
		internal_error("P2A: lambda parms expr expected");
	    }
	}
    }
    return parse_simple_owner(x);
}

static Pragma_list extract_on_pragmas(
    Pragma_list p_l, OwnerExpr_list *o_l, OwnerExpr(*f)(PragmaExpression))
{
    *o_l = OwnerExpr_listNIL;

    if (p_l == Pragma_listNIL)
	return p_l;

    for (unsigned i = 0; i < p_l->sz;)
    {
	Pragma p = p_l->arr[i];
	if (p->name->sym == add_tmsymbol("on"))
	{
	    p_l->arr[i] = PragmaNIL;
	    p_l = delete_Pragma_list(p_l, i);
	    assert (p->tag == TAGValuePragma);
	    PragmaExpression pe = to_ValuePragma(p)->x;
	    if ((*o_l) == OwnerExpr_listNIL)
		*o_l = new_OwnerExpr_list();
	    *o_l = append_OwnerExpr_list(*o_l, f(pe));
	    rfre_Pragma(p);
	}
	else
	{
	    i++;
	}
    }
    return p_l;
}

Pragma_list extract_simple_on_pragmas(Pragma_list p_l, OwnerExpr_list *o_l)
{
    return extract_on_pragmas(p_l, o_l, parse_simple_owner);
}

Pragma_list extract_abstract_on_pragmas(Pragma_list p_l, OwnerExpr_list *o_l)
{
    return extract_on_pragmas(p_l, o_l, parse_abstract_owner);
}
