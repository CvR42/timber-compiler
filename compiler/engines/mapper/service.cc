#include <assert.h>
#include <tmc.h>

#include "defs.h"
#include "tmadmin.h"
#include "vnusctl.h"
#include "global.h"
#include "error.h"
#include "service.h"
#include "search.h"
#include "replace.h"


void init_global_context(global_context gc)
{
    gc->prog = NULL;
    gc->processorTypes = NULL;
    gc->processors = NULL;
    gc->numberOfProcessors = 0;
    gc->prelst = NULL;
    gc->add_to_prelst = false;
    gc->scope = NULL;
    gc->current_parms = NULL;
    gc->do_statements = false;
    gc->current_ass = NULL;
    gc->current_owner = NULL;
    gc->current_pragmas = NULL;
    gc->current_varowner = NULL;
    gc->current_indices = NULL;
    gc->visited = new_origsymbol_list();
    gc->current_cs_f_ir = DeclLocalVariableNIL;
    gc->current_tir = DeclRecordNIL;
}

void destroy_global_context(global_context gc)
{
    if (gc->processorTypes != NULL)
    {
	rfre_ProcessorType_list(gc->processorTypes);
    }
    if (gc->processors != NULL)
    {
	rfre_ProcessorDecl_list(gc->processors);
    }
    rfre_origsymbol_list(gc->visited);
}

int pragma_list_find_index_tmsymbol(Pragma_list pl, tmsymbol tms)
{
    if (pl == Pragma_listNIL)
        return -1;
    for (unsigned i=0; i<pl->sz; i++)
        if (pl->arr[i]->name->sym == tms)
            return i;
    return -1;
}


int pragma_list_contains_tmsymbol(Pragma_list pl, tmsymbol tms)
{
    return (pragma_list_find_index_tmsymbol(pl, tms) != -1);
}

int declaration_list_find_pos(declaration_list dl, origsymbol s)
{
    for (unsigned i = 0; i < dl->sz; i++)
        if (dl->arr[i] != declarationNIL && dl->arr[i]->name->sym == s->sym)
            return i;
    return -1;
}

declaration declaration_list_find_origsymbol(declaration_list dl, origsymbol s)
{
    for (unsigned i = 0; i < dl->sz; i++)
        if (dl->arr[i] != declarationNIL && dl->arr[i]->name->sym == s->sym)
            return dl->arr[i];
    return NULL;
}

declaration declaration_list_find_pragma(declaration_list dl, tmsymbol s)
{
    for (unsigned i = 0; i < dl->sz; i++)
        if (dl->arr[i] != declarationNIL && pragma_list_contains_tmsymbol( dl->arr[i]->pragmas, s))
            return dl->arr[i];
    return NULL;
}

field field_list_find_origsymbol(field_list fl, origsymbol s)
{
    for (unsigned i = 0; i < fl->sz; i++)
        if (fl->arr[i]->name->sym == s->sym)
            return fl->arr[i];
    return NULL;
}

int find_field( const declaration_list decls, const type rectype, const origsymbol field )
{
    const_field_list fields;
    int res = 0;

    switch( rectype->tag ){
	case TAGTypeNamedRecord:
	    fields = lookup_NamedRecord_fields(
		decls,
		to_TypeNamedRecord(rectype)->name
	    );
	    res = find_field_fields( fields, field );
	    break;

	case TAGTypeRecord:
	    fields = to_TypeRecord(rectype)->fields;
	    res =  find_field_fields( fields, field );
	    break;

	case TAGTypeBase:
	case TAGTypeShape:
	case TAGTypePointer:
	case TAGTypeProcedure:
	case TAGTypeFunction:
	case TAGTypeNeutralPointer:
	case TAGTypeArray:
	case TAGTypeMap:
	case TAGTypePragmas:
	case TAGTypeUnsizedArray:
            
	    internal_error( "can only find the field of a record type" );
	    return 0;
    }
    return res;
}

location find_owneron_name ( const declaration_list decls, origsymbol n )
{
    declaration d = declaration_list_find_origsymbol(decls, n);
    if (d == NULL)
        return NULL;
    if (d->tag != TAGDeclLocalVariable)
        return NULL;
    DeclLocalVariable dlv = to_DeclLocalVariable(d);
    type theType = dlv->t;
    theType = pierceTypePragmas(theType);
    if (theType->tag != TAGTypeMap)
        return NULL;
    TypeMap tm = to_TypeMap(theType);
    OwnerExpr map = tm->map;
    if (map->tag != TAGNewOwnLocation)
        return NULL;
    NewOwnLocation nol = to_NewOwnLocation(map);
    if (nol->distr->sz != 1)
        return NULL;
    if (nol->distr->arr[0]->tag != TAGNewDistAlign)
        return NULL;
    NewDistAlign oo = to_NewDistAlign(nol->distr->arr[0]);
    return oo->with;
}

/*
  Testing is a bit limited here, but this function is only intended for
  use with tempos, which are used by name anyway.
  */
location find_owneron_location ( const declaration_list decls, location l )
{
    switch (l->tag)
    {
        case TAGLocName:
        {
            LocName ln = to_LocName(l);
	    return find_owneron_name(decls, ln->name);
            break;
        }
        case TAGLocSelection:
        {
            LocSelection ls = to_LocSelection(l);
            if (ls->shape->tag != TAGExprName)
                return locationNIL;
            ExprName en = to_ExprName(ls->shape);
            return find_owneron_name(decls, en->name);
            break;
        }
	case TAGLocWrapper:
	{
	    LocWrapper lo = to_LocWrapper(l);
	    assert (lo->on->sz == 1);
	    assert (lo->on->arr[0]->tag = TAGNewOwnLocation);
	    NewOwnLocation nol = to_NewOwnLocation(lo->on->arr[0]);
	    assert (nol->proc == expressionNIL);
	    assert (nol->distr->sz == 1);
	    assert (nol->distr->arr[0]->tag == TAGNewDistAlign);
	    return to_NewDistAlign(nol->distr->arr[0])->with;
	}
        default:
            return locationNIL;
            break;
    }
}


// Given a list of declarations 'defs' and a type 't', return a copy
// of the list of fields of the type.
field_list get_record_fields( declaration_list defs, type t )
{
    switch( t->tag ){
	case TAGTypeRecord:
	    return rdup_field_list( to_TypeRecord( t )->fields );

	case TAGTypeNamedRecord:
	    return rdup_field_list(
		lookup_NamedRecord_fields( defs, to_TypeNamedRecord(t)->name )
	    );

	case TAGTypeBase:
	case TAGTypeShape:
	case TAGTypePointer:
	case TAGTypeProcedure:
	case TAGTypeFunction:
	case TAGTypeArray:
	case TAGTypeNeutralPointer:
	case TAGTypeMap:
	case TAGTypePragmas:
	case TAGTypeUnsizedArray:
	    break;
    }
    return field_listNIL;
}

/*
  Delete an entry in the list, and set that entry to NIL. The list
  is not shifted to the left to prevent problems with some engines.
  */
void mark_declaration_list(declaration_list l, unsigned pos)
{
    assert(pos < l->sz);
    rfre_declaration(l->arr[pos]);
    l->arr[pos] = declarationNIL;
}

/*
  When an engine has used the previous function to delete list entries
  is should, when it is finished, call this function, as there might be
  other code around that cannot handle NILs in lists.
  All entries in the list are shifted to the left to fill in all the
  gaps where NILs were.
  */
declaration_list delete_marked_declaration_list(declaration_list l)
{
    unsigned i = 0, delta = 0;
    while(i < l->sz)
    {
        if (l->arr[i+delta] == declarationNIL)
        {
            delta++;
            l->sz--;
        }
        else
        {
            l->arr[i] = l->arr[i+delta];
            i++;
        }
    }
    return l;
}

int isIntValue(expression e, const int v)
{
    return ((e->tag == TAGExprInt) &&
            (to_ExprInt(e)->v == v));
}

int isBooleanValue(expression e, vnus_boolean b)
{
    return ((e->tag == TAGExprBoolean) &&
            (to_ExprBoolean(e)->b == b));
        
}

type pierceTypeMap(type t)
{
    assert (t != typeNIL);
    while (t->tag == TAGTypeMap)
    {
	if (warning_level >= WARN_DEBUG)
	    fprintf(stderr, "Service: one TypeMap pierced\n");
        t = to_TypeMap(t)->t;
    }
    return t;
}

type pierceTypePragmas(type t)
{
    assert (t != typeNIL);
    while (t->tag == TAGTypePragmas)
    {
	if (warning_level >= WARN_DEBUG)
	    fprintf(stderr, "Service: one TypePragmas pierced\n");
        t = to_TypePragmas(t)->t;
    }
    return t;
}

type pierceTypeMapAndPragmas(type t)
{
    assert (t != typeNIL);
    int done = false;
    while (!done)
    {
        switch (t->tag)
        {
            case TAGTypeMap:
		if (warning_level >= WARN_DEBUG)
		    fprintf(stderr, "Service: one TypeMap pierced\n");
                t = to_TypeMap(t)->t;
                break;
            case TAGTypePragmas:
		if (warning_level >= WARN_DEBUG)
		    fprintf(stderr, "Service: one TypePragmas pierced\n");
                t = to_TypePragmas(t)->t;
                break;
            default:
                done = true;
                break;
        }
    }
    return t;
}

location pierceLocationWrapper(location l)
{
    assert (l != locationNIL);
    int done = false;
    while (!done)
    {
        switch (l->tag)
        {
            case TAGLocWrapper:
		if (warning_level >= WARN_DEBUG)
		    fprintf(stderr, "Service: one LocWrapper pierced\n");
                l = to_LocWrapper(l)->l;
                break;
            default:
                done = true;
                break;
        }
    }
    return l;
}

expression pierceExprWrapper(expression e)
{
    assert (e != expressionNIL);
    int done = false;
    while (!done)
    {
        switch (e->tag)
        {
            case TAGExprWrapper:
		if (warning_level >= WARN_DEBUG)
		    fprintf(stderr, "Service: one ExprWrapper pierced\n");
                e = to_ExprWrapper(e)->x;
                break;
            default:
                done = true;
                break;
        }
    }
    return e;
}

expression pierceExprNotNullAssert(
    expression e)
{
    assert (e != expressionNIL);
    switch (e->tag)
    {
	case TAGExprNotNullAssert:
	    return pierceExprNotNullAssert(to_ExprNotNullAssert(e)->x);
	default:
	    return e;
    }
}

#if 0
static type getVarType(declaration d)
{
    assert (d != declarationNIL);
    type t = typeNIL;
    switch (d->tag)
    {
        case TAGDeclGlobalVariable:
            t = to_DeclGlobalVariable(d)->t;
            break;
        case TAGDeclLocalVariable:
            t = to_DeclLocalVariable(d)->t;
            break;
        case TAGDeclReturnVariable:
            t = to_DeclReturnVariable(d)->t;
            break;
        case TAGDeclFormalVariable:
            t = to_DeclFormalVariable(d)->t;
            break;
        case TAGDeclExternalVariable:
            t = to_DeclExternalVariable(d)->t;
            break;
        default:
            break;
    }
    return t;
}
#endif

int hasDistribution(type t, global_context gc)
{
    assert (t != typeNIL);
    switch (t->tag)
    {
	case TAGTypeMap:
	{
	    TypeMap tm = to_TypeMap(t);
	    return (true &&
		    hasDistribution(tm->t, gc));
	    break;
	}
	case TAGTypeBase:
	    return false;
	    break;
	case TAGTypeShape:
	{
	    TypeShape ts = to_TypeShape(t);
	    return hasDistribution(ts->elmtype, gc);
	    break;
	}
	case TAGTypeUnsizedArray:
	    return hasDistribution(to_TypeUnsizedArray(t)->elmtype, gc);
	    break;
	case TAGTypePointer:
	    return hasDistribution(to_TypePointer(t)->elmtype, gc);
	    break;
	case TAGTypeNeutralPointer:
	    return false;
	    break;
	case TAGTypeArray:
	    return hasDistribution(to_TypeArray(t)->elmtype, gc);
	    break;
	case TAGTypeRecord:
	{
	    TypeRecord tr = to_TypeRecord(t);
	    if (tr->fields != field_listNIL)
		for (unsigned i=0; i<tr->fields->sz; i++)
		    if (hasDistribution(tr->fields->arr[i]->elmtype, gc))
			return true;
	    return false;
	    break;
	}
	case TAGTypeNamedRecord:
	{
		/*
		  We need 'gc->visited' to check for recusion.
		  Recursion is not handled correctly  in the case of
		  a->b->a, as the result for be might be negative instead of
		  positive because a is not yet known.
		  */
	    TypeNamedRecord tr = to_TypeNamedRecord(t);
	    assert(gc->visited != origsymbol_listNIL);
	    for (unsigned i=0; i<gc->visited->sz; i++)
	    {
		if (gc->visited->arr[i]->sym == tr->name->sym)
		    return false;
	    }
	    gc->visited = append_origsymbol_list(gc->visited, rdup_origsymbol(tr->name));
		// Need to find declaration
	    declaration d = declaration_list_find_origsymbol(gc->prog->declarations, tr->name);
	    assert (d != declarationNIL);
	    assert (d->tag == TAGDeclRecord);
	    DeclRecord dr = to_DeclRecord(d);
	    if (dr->fields != field_listNIL)
		for (unsigned i=0; i<dr->fields->sz; i++)
		{
		    if (hasDistribution(dr->fields->arr[i]->elmtype, gc))
		    {
			gc->visited = delete_origsymbol_list(gc->visited, gc->visited->sz-1);
			return true;
		    }
		}
	    gc->visited = delete_origsymbol_list(gc->visited, gc->visited->sz-1);
	    return false;
	    break;
	}
	case TAGTypeProcedure:
	{
	    TypeProcedure tp = to_TypeProcedure(t);
	    if (tp->formals != type_listNIL)
		for (unsigned i=0; i<tp->formals->sz; i++)
		    if (hasDistribution(tp->formals->arr[i], gc))
			return true;
	    return false;
	    break;
	}
	case TAGTypeFunction:
	{
	    TypeFunction tf = to_TypeFunction(t);
	    if (hasDistribution(tf->rettype, gc))
		return true;
	    if (tf->formals != type_listNIL)
		for (unsigned i=0; i<tf->formals->sz; i++)
		    if (hasDistribution(tf->formals->arr[i], gc))
			return true;
	    return false;
	    break;
	}
	case TAGTypePragmas:
	    return hasDistribution(to_TypePragmas(t)->t, gc);
	    break;
    }
    internal_error("hasDistribution: reached unreachable code");
    return false;
}

int isReplicated(type t, global_context gc)
{
    assert (t != typeNIL);
    assert (t->tag == TAGTypeShape);
    assert (hasDistribution(t, gc));
#if 0    
    TypeShape ts = to_TypeShape(t);
    for (unsigned i = 0; i<ts->distr->sz; i++)
    {
        if (ts->distr->arr[i]->tag == TAGDistReplicated)
            return true;
    }
#endif
    return false;
}

bool containsReplicated(OwnerExpr_list ol)
{
    if (ol == OwnerExpr_listNIL)
	return false;
    for (unsigned i=0; i<ol->sz; i++)
    {
	if (containsReplicated(ol->arr[i]))
	    return true;
    }
    return false;
}

bool containsReplicated(OwnerExpr o)
{
    assert (o != OwnerExprNIL);
    switch (o->tag)
    {
	case TAGNewOwnDontCare:
	    break;
	case TAGNewOwnReplicated:
	    return true;
	case TAGNewOwnLocation:
	{
	    NewOwnLocation nol = to_NewOwnLocation(o);
	    if (nol->distr != DistrExpr_listNIL)
	    {
		for (unsigned j=0; j<nol->distr->sz; j++)
		{
		    switch (nol->distr->arr[j]->tag)
		    {
			case TAGNewDistReplicated:
			    return true;
			case TAGNewDistDontCare:
			case TAGNewDistBlock:
			case TAGNewDistCyclic:
			case TAGNewDistBC:
			case TAGNewDistLocal:
			case TAGNewDistAlign:
			    break;
		    }
		}
	    }
	    break;
	}
	case TAGNewOwnMap:
	    NewOwnMap nom = to_NewOwnMap(o);
	    bool flag = containsReplicated(nom->on);
	    if (flag)
		return true;
	    break;
    }
    return false;
}


cardinality_list normalise_cardinalities(cardinality_list l, global_context gc)
{
    assert (l != cardinality_listNIL);
    for (unsigned i=0; i<l->sz; i++)
    {
        origsymbol loopVar = l->arr[i]->name;
        expression lowerBound = l->arr[i]->lowerbound;
        expression upperBound = l->arr[i]->upperbound;
        expression stride = l->arr[i]->stride;
        if (!isIntValue(lowerBound, 0) || !isIntValue(stride, 1))
        {
	    if (warning_level >= WARN_DEBUG)
		fprintf(stderr, "Service: normalising cardinality %d:%s\n",i,loopVar->sym->name);
            DeclCardinalityVariable nd = new_DeclCardinalityVariable(
                gen_origsymbol("__cvn"),
                0,
                Pragma_listNIL);
            gc->prog->declarations = append_declaration_list(gc->prog->declarations, nd);
            cardinality nc = new_cardinality(
                rdup_origsymbol(nd->name),
                new_ExprInt(0),
                new_ExprBinop(
                    new_ExprBinop(
                        new_ExprBinop(
                            rdup_expression(upperBound),
                            BINOP_MINUS,
                            rdup_expression(lowerBound)),
                        BINOP_PLUS,
                        new_ExprBinop(
                            rdup_expression(stride),
                            BINOP_MINUS,
                            new_ExprInt(1))),
                    BINOP_DIVIDE,
                    rdup_expression(stride)),
                new_ExprInt(1),
                rdup_secondary_list(l->arr[i]->secondaries));
            secondary ns = new_secondary(
                rdup_origsymbol(loopVar),
                rdup_expression(lowerBound),
                rdup_expression(stride));
            nc->secondaries = insert_secondary_list(
                nc->secondaries,
                0,
                ns);
            rfre_cardinality(l->arr[i]);
            l->arr[i] = nc;
        }
    }
    return l;
}

/*
  This is a very simple test to see if an expression is lineair.
  Can and should be much more advanced.
  This needs more structure. Too much cut'n paste.
  Should use normalise_, split_ and form_ExprBinop functions.
  */
int determine_lineair(
    expression AIBexpr,
    origsymbol loopVar,
    expression *A,
    expression *B)
{
    if (AIBexpr->tag == TAGExprName)
    {
        ExprName en = to_ExprName(AIBexpr);
        if (en->name->sym == loopVar->sym)
        {
	    if (warning_level >= WARN_DEBUG)
		fprintf(stderr,"Service:yup:1i+0 in %s\n",en->name->sym->name);
            *A = new_ExprInt(1);
            *B = new_ExprInt(0);
            return true;
        }
    }
    if ((AIBexpr->tag == TAGExprBinop) && (to_ExprBinop(AIBexpr)->optor == BINOP_PLUS))
    {
        ExprBinop ebo = to_ExprBinop(AIBexpr);
        if ((ebo->left->tag == TAGExprBinop) && (to_ExprBinop(ebo->left)->optor == BINOP_TIMES))
        {
            ExprBinop ebo2 = to_ExprBinop(ebo->left);
            if (ebo2->right->tag == TAGExprName)
            {
                ExprName en = to_ExprName(ebo2->right);
                if ((en->name->sym == loopVar->sym) &&
                    (!search_expression(ebo->right,loopVar->sym)) &&
                    (!search_expression(ebo2->left,loopVar->sym)))
                {
		    if (warning_level >= WARN_DEBUG)
			fprintf(stderr,"Service:yup:ai+b in %s\n",en->name->sym->name);
                    *A = rdup_expression(ebo2->left);
                    *B = rdup_expression(ebo->right);
                    return true;
                }
            }
        }
        if (ebo->left->tag == TAGExprName)
        {
            ExprName en = to_ExprName(ebo->left);
            if ((en->name->sym == loopVar->sym) &&
                (!search_expression(ebo->right,loopVar->sym)))
            {
		if (warning_level >= WARN_DEBUG)
		    fprintf(stderr,"Service:yup:1i+b in %s\n",en->name->sym->name);
                *A = new_ExprInt(1);
                *B = rdup_expression(ebo->right);
                return true;
            }
        }
    }
    if ((AIBexpr->tag == TAGExprBinop) && (to_ExprBinop(AIBexpr)->optor == BINOP_MINUS))
    {
        ExprBinop ebo = to_ExprBinop(AIBexpr);
        if ((ebo->left->tag == TAGExprBinop) && (to_ExprBinop(ebo->left)->optor == BINOP_TIMES))
        {
            ExprBinop ebo2 = to_ExprBinop(ebo->left);
            if (ebo2->right->tag == TAGExprName)
            {
                ExprName en = to_ExprName(ebo2->right);
                if ((en->name->sym == loopVar->sym) &&
                    (!search_expression(ebo->right,loopVar->sym)) &&
                    (!search_expression(ebo2->left,loopVar->sym)))
                {
		    if (warning_level >= WARN_DEBUG)
			fprintf(stderr,"Service:yup:ai-b in %s\n",en->name->sym->name);
                    *A = rdup_expression(ebo2->left);
                    *B = new_ExprUnop(
			UNOP_NEGATE,
			rdup_expression(ebo->right));
                    return true;
                }
            }
        }
        if (ebo->left->tag == TAGExprName)
        {
            ExprName en = to_ExprName(ebo->left);
            if ((en->name->sym == loopVar->sym) &&
                (!search_expression(ebo->right,loopVar->sym)))
            {
		if (warning_level >= WARN_DEBUG)
		    fprintf(stderr,"Service:yup:1i-b in %s\n",en->name->sym->name);
                *A = new_ExprInt(1);
		*B = new_ExprUnop(
		    UNOP_NEGATE,
		    rdup_expression(ebo->right));
                return true;
            }
        }
    }
    if ((AIBexpr->tag == TAGExprBinop) && (to_ExprBinop(AIBexpr)->optor == BINOP_TIMES))
    {
        ExprBinop ebo = to_ExprBinop(AIBexpr);
        if (ebo->right->tag == TAGExprName)
        {
            ExprName en = to_ExprName(ebo->right);
            if ((en->name->sym == loopVar->sym) &&
                (!search_expression(ebo->left,loopVar->sym)))
            {
		if (warning_level >= WARN_DEBUG)
		    fprintf(stderr,"Service:yup:ai+0 in %s\n",en->name->sym->name);
                *A = rdup_expression(ebo->left);
                *B = new_ExprInt(0);
                return true;
            }
        }
    }
    *A = expressionNIL;
    *B = expressionNIL;
    return (false);
}

#if 0
/*
  Determine if a condition is an IsOwner or Sender expression.
  If so, return the shape they are operating on.
  */
LocSelection extract_LocSelection(
    expression cond,
    expression *procExpr)
{
    LocSelection lsel = LocSelectionNIL;
    expression proc = expressionNIL;
    location shape = locationNIL;
    
    switch (cond->tag)
    {
        case TAGExprIsOwner:
        {
	    if (warning_level >= WARN_DEBUG)
		fprintf(stderr,"Service: possible IsOwner!!!\n");
            ExprIsOwner xio = to_ExprIsOwner(cond);
            proc = xio->proc;
            shape = xio->shape;
            break;
        }
        case TAGExprBinop:
        {
            ExprBinop xbop = to_ExprBinop(cond);
            if ((xbop->optor == BINOP_EQUAL) && (xbop->left->tag == TAGExprSender))
            {
		if (warning_level >= WARN_DEBUG)
		    fprintf(stderr,"Service: possible Sender!!!\n");
                ExprSender xs = to_ExprSender(xbop->left);
                proc = xbop->right;
                shape = xs->shape;
            }
            break;
        }
        default:
            break;
    }
    if (shape != locationNIL)
    {
        shape = pierceLocationWrapper(shape);
        if (shape->tag == TAGLocSelection)
        {
	    if (warning_level >= WARN_DEBUG)
		fprintf(stderr,"Service: yup, got selectionlocation\n");
            lsel = to_LocSelection(rdup_location(shape));
            proc = rdup_expression(proc);
        }
    }
    *procExpr = proc;
    return lsel;
}
#endif

/*
  Normalise a binary expression to the form (a op (b op (c ...)))
  */
expression normalise_ExprBinop(
    expression e,
    BINOP op)
{
    if (e->tag != TAGExprBinop)
        return e;
    ExprBinop top = to_ExprBinop(e);
    if (top->optor != op)
        return e;
    top->right = normalise_ExprBinop(top->right, op);
    if (top->left->tag != TAGExprBinop)
        return e;
    ExprBinop topLeft = to_ExprBinop(top->left);
    if (topLeft->optor != op)
        return e;
    top->left = topLeft->right;
    topLeft->right = top;
    return normalise_ExprBinop(topLeft, op);
}

/*
  Take an expression of the form (a op (b op (c ...)))
  and produce a list {a,b,c}
  */
ExprReduction split_ExprBinop(
    expression e,
    BINOP op)
{
    expression_list l = new_expression_list();
    expression ptr = e;
    while (ptr->tag == TAGExprBinop && to_ExprBinop(ptr)->optor == op)
    {
        ExprBinop ebo = to_ExprBinop(ptr);
        l = append_expression_list(l, rdup_expression(ebo->left));
        ptr = ebo->right;
    }
    l = append_expression_list(l, rdup_expression(ptr));
    return new_ExprReduction(op, l);
}

/*
  Take a list of the form {a,b,c}
  and produce an expression (a op (b op (c ...)))
  */
expression form_ExprBinop(
    ExprReduction er)
{
    assert (er->operands != expression_listNIL && er->operands->sz != 0);
    expression e = rdup_expression(er->operands->arr[er->operands->sz-1]);
    for (int i=er->operands->sz-2; i>=0; i--)
    {
        e = new_ExprBinop(
            rdup_expression(er->operands->arr[i]),
            er->optor,
            e);
    }
    return e;
}

#if 0
distribution_list extract_distribution(
    global_context gc,
    expression shape)
{
    type shapeType = typeNIL;
    distribution_list distr = distribution_listNIL;
    if (warning_level >= WARN_DEBUG)
	fprintf(stderr, "Service: now to find out distribution...\n");
    shapeType = derive_type_expression(gc->prog->declarations, 0, shape);
    assert (shapeType->tag == TAGTypeShape);
    if (warning_level >= WARN_DEBUG)
	fprintf(stderr, "Service: found shape type\n");
    if (hasDistribution(shapeType, gc))
    {
	if (warning_level >= WARN_DEBUG)
	    fprintf(stderr, "Service: found non-empty distr\n");
        distr = rdup_distribution_list(to_TypeShape(shapeType)->distr);
    }
    rfre_type(shapeType);
    return distr;
}
#endif

int count_real_distributions(
    DistrExpr_list distr)
{
    int noRealDistributions = 0;
    for (unsigned i=0; i<distr->sz; i++)
    {
        switch (distr->arr[i]->tag)
        {
            case TAGNewDistDontCare:
            case TAGNewDistBlock:
            case TAGNewDistCyclic:
            case TAGNewDistBC:
            case TAGNewDistReplicated:
            case TAGNewDistLocal:
                noRealDistributions++;
                break;
            case TAGNewDistAlign:
                break;
        }
    }
    return noRealDistributions;
}

int count_local_distributions(
    DistrExpr_list distr)
{
    int nolocalDistributions = 0;
    for (unsigned i=0; i<distr->sz; i++)
    {
        switch (distr->arr[i]->tag)
        {
            case TAGNewDistDontCare:
            case TAGNewDistBlock:
            case TAGNewDistCyclic:
            case TAGNewDistBC:
            case TAGNewDistReplicated:
            case TAGNewDistAlign:
                break;
            case TAGNewDistLocal:
                nolocalDistributions++;
                break;
        }
    }
    return nolocalDistributions;
}

int count_iffy_distributions(
    DistrExpr_list distr)
{
    int noIffyDistributions = 0;
    for (unsigned i=0; i<distr->sz; i++)
    {
        switch (distr->arr[i]->tag)
        {
            case TAGNewDistDontCare:
            case TAGNewDistReplicated:
            case TAGNewDistAlign:
                noIffyDistributions++;
                break;
            case TAGNewDistLocal:
            case TAGNewDistBlock:
            case TAGNewDistCyclic:
            case TAGNewDistBC:
                break;
        }
    }
    return noIffyDistributions;
}

void report_distributions(
    DistrExpr_list distr)
{
    if (warning_level < WARN_DEBUG)
	return;
    for (unsigned i=0; i<distr->sz; i++)
    {
        switch (distr->arr[i]->tag)
        {
            case TAGNewDistBlock:
                fprintf(stderr,"Service:block dist\n");
                break;
            case TAGNewDistCyclic:
                fprintf(stderr,"Service:cyclic dist\n");
                break;
            case TAGNewDistBC:
                fprintf(stderr,"Service:blockcyclic dist\n");
                break;
            case TAGNewDistLocal:
                fprintf(stderr,"Service:local dist\n");
                break;
            case TAGNewDistReplicated:
                fprintf(stderr,"Service:replicated dist\n");
                break;
            case TAGNewDistDontCare:
                fprintf(stderr,"Service:dontcare dist\n");
                break;
            case TAGNewDistAlign:
                fprintf(stderr,"Service:align dist\n");
                break;
        }
    }
}

//This only works if lineair in only one primary or secondary cardinality variable.
void extract_lineair(
    expression AIBexpr,
    cardinality card,
    expression *A,
    expression *B)
{
    if (AIBexpr->tag == TAGExprWrapper)
    {
	return extract_lineair(to_ExprWrapper(AIBexpr)->x,card,A,B);
    }
    expression exprA = expressionNIL;
    expression exprB = expressionNIL;
    if (!determine_lineair(AIBexpr,card->name,&exprA,&exprB))
    {
        assert (exprA ==expressionNIL);
        assert (exprB ==expressionNIL);
	if (warning_level >= WARN_DEBUG)
	    fprintf(stderr,"Service:nope:complicated expression in %s\n", card->name->sym->name);
	if (card->secondaries != secondary_listNIL)
	{
	    for (unsigned i=0; i<card->secondaries->sz;i++)
	    {
		expression localA = expressionNIL;
		expression localB = expressionNIL;
		if (determine_lineair(AIBexpr,card->secondaries->arr[i]->name,&localA,&localB))
		{
		    if (warning_level >= WARN_DEBUG)
			fprintf(stderr,"Service:bummer, lineair in secondary\n");
		    exprA = new_ExprBinop(
			rdup_expression(localA),
			BINOP_TIMES,
			rdup_expression(card->secondaries->arr[i]->stride));
		    exprB = new_ExprBinop(
			new_ExprBinop(
			    rdup_expression(localA),
			    BINOP_TIMES,
			    rdup_expression(card->secondaries->arr[i]->lowerbound)),
			BINOP_PLUS,
			rdup_expression(localB));
		    rfre_expression(localA);
		    rfre_expression(localB);
		    break;
		} else {
		    if (warning_level >= WARN_DEBUG)
			fprintf(stderr,"Service:nope:complicated expression in %s\n", card->secondaries->arr[i]->name->sym->name);
		}
		assert (localA ==expressionNIL);
		assert (localB ==expressionNIL);
	    }
	}
    }
    *A = exprA;
    *B = exprB;
}

expression replace_expression_indices(
    global_context gc, expression e, cardinality_list old_l, cardinality_list new_l)
{
    for (unsigned i=0; i<old_l->sz; i++)
    {
        tmsymbol findSym = old_l->arr[i]->name->sym;
        expression replaceExpr = new_ExprName(rdup_origsymbol(new_l->arr[i]->name));
        location replaceLoc = new_LocName(rdup_origsymbol(new_l->arr[i]->name));
        e = replace_expression(e, findSym, replaceExpr, replaceLoc, gc);
        rfre_expression(replaceExpr);
        rfre_location(replaceLoc);
        for (unsigned j=0; j< old_l->arr[i]->secondaries->sz; j++)
        {
            findSym = old_l->arr[i]->secondaries->arr[j]->name->sym;
            replaceExpr = new_ExprName(rdup_origsymbol(new_l->arr[i]->secondaries->arr[j]->name));
            replaceLoc = new_LocName(rdup_origsymbol(new_l->arr[i]->secondaries->arr[j]->name));
            e = replace_expression(e, findSym, replaceExpr, replaceLoc, gc);
            rfre_expression(replaceExpr);
            rfre_location(replaceLoc);
        }
    }
    return e;
}

location replace_location_indices(
    global_context gc, location e, cardinality_list old_l, cardinality_list new_l)
{
    for (unsigned i=0; i<old_l->sz; i++)
    {
        tmsymbol findSym = old_l->arr[i]->name->sym;
        expression replaceExpr = new_ExprName(rdup_origsymbol(new_l->arr[i]->name));
        location replaceLoc = new_LocName(rdup_origsymbol(new_l->arr[i]->name));
        e = replace_location(e, findSym, replaceExpr, replaceLoc, gc);
        rfre_expression(replaceExpr);
        rfre_location(replaceLoc);
        for (unsigned j=0; j< old_l->arr[i]->secondaries->sz; j++)
        {
            findSym = old_l->arr[i]->secondaries->arr[j]->name->sym;
            replaceExpr = new_ExprName(rdup_origsymbol(new_l->arr[i]->secondaries->arr[j]->name));
            replaceLoc = new_LocName(rdup_origsymbol(new_l->arr[i]->secondaries->arr[j]->name));
            e = replace_location(e, findSym, replaceExpr, replaceLoc, gc);
            rfre_expression(replaceExpr);
            rfre_location(replaceLoc);
        }
    }
    return e;
}

cardinality_list replace_cardinality_list_indices(
    global_context gc, cardinality_list e, cardinality_list old_l, cardinality_list new_l)
{
    for (unsigned i=0; i<old_l->sz; i++)
    {
        tmsymbol findSym = old_l->arr[i]->name->sym;
        expression replaceExpr = new_ExprName(rdup_origsymbol(new_l->arr[i]->name));
        location replaceLoc = new_LocName(rdup_origsymbol(new_l->arr[i]->name));
        e = replace_cardinality_list(e, findSym, replaceExpr, replaceLoc, gc);
        rfre_expression(replaceExpr);
        rfre_location(replaceLoc);
        for (unsigned j=0; j< old_l->arr[i]->secondaries->sz; j++)
        {
            findSym = old_l->arr[i]->secondaries->arr[j]->name->sym;
            replaceExpr = new_ExprName(rdup_origsymbol(new_l->arr[i]->secondaries->arr[j]->name));
            replaceLoc = new_LocName(rdup_origsymbol(new_l->arr[i]->secondaries->arr[j]->name));
            e = replace_cardinality_list(e, findSym, replaceExpr, replaceLoc, gc);
            rfre_expression(replaceExpr);
            rfre_location(replaceLoc);
        }
    }
    return e;
}

OwnerExpr replace_OwnerExpr_indices(
    global_context gc, OwnerExpr e, cardinality_list old_l, cardinality_list new_l)
{
    for (unsigned i=0; i<old_l->sz; i++)
    {
        tmsymbol findSym = old_l->arr[i]->name->sym;
        expression replaceExpr = new_ExprName(rdup_origsymbol(new_l->arr[i]->name));
        location replaceLoc = new_LocName(rdup_origsymbol(new_l->arr[i]->name));
        e = replace_OwnerExpr(e, findSym, replaceExpr, replaceLoc, gc);
        rfre_expression(replaceExpr);
        rfre_location(replaceLoc);
        for (unsigned j=0; j< old_l->arr[i]->secondaries->sz; j++)
        {
            findSym = old_l->arr[i]->secondaries->arr[j]->name->sym;
            replaceExpr = new_ExprName(rdup_origsymbol(new_l->arr[i]->secondaries->arr[j]->name));
            replaceLoc = new_LocName(rdup_origsymbol(new_l->arr[i]->secondaries->arr[j]->name));
            e = replace_OwnerExpr(e, findSym, replaceExpr, replaceLoc, gc);
            rfre_expression(replaceExpr);
            rfre_location(replaceLoc);
        }
    }
    return e;
}

statement replace_statement_indices(
    global_context gc, statement e, cardinality_list old_l, cardinality_list new_l)
{
    for (unsigned i=0; i<old_l->sz; i++)
    {
        tmsymbol findSym = old_l->arr[i]->name->sym;
        expression replaceExpr = new_ExprName(rdup_origsymbol(new_l->arr[i]->name));
        location replaceLoc = new_LocName(rdup_origsymbol(new_l->arr[i]->name));
        e = replace_statement(e, findSym, replaceExpr, replaceLoc, gc);
        rfre_expression(replaceExpr);
        rfre_location(replaceLoc);
        for (unsigned j=0; j< old_l->arr[i]->secondaries->sz; j++)
        {
            findSym = old_l->arr[i]->secondaries->arr[j]->name->sym;
            replaceExpr = new_ExprName(rdup_origsymbol(new_l->arr[i]->secondaries->arr[j]->name));
            replaceLoc = new_LocName(rdup_origsymbol(new_l->arr[i]->secondaries->arr[j]->name));
            e = replace_statement(e, findSym, replaceExpr, replaceLoc, gc);
            rfre_expression(replaceExpr);
            rfre_location(replaceLoc);
        }
    }
    return e;
}

cardinality_list unique_rdup_cardinality_list(
    global_context gc, cardinality_list l)
{
    cardinality_list ret = rdup_cardinality_list(l);
    for (unsigned i=0; i<ret->sz; i++)
    {
        rfre_origsymbol(ret->arr[i]->name);
        ret->arr[i]->name = gen_origsymbol("__unique_cv_");
        for (unsigned j=0; j< ret->arr[i]->secondaries->sz; j++)
        {
            rfre_origsymbol(ret->arr[i]->secondaries->arr[j]->name);
            ret->arr[i]->secondaries->arr[j]->name = gen_origsymbol("__unique_cv_");
        }
    }
    ret = replace_cardinality_list_indices(gc,ret,l,ret);
    return ret;
}

void add_cardinality_declarations(
    global_context gc, cardinality_list l)
{
    for (unsigned i=0; i<l->sz; i++)
    {
        gc->prog->declarations = append_declaration_list(
            gc->prog->declarations,
            new_DeclCardinalityVariable(
                rdup_origsymbol(l->arr[i]->name),
                0,
                new_Pragma_list()));
        for (unsigned j=0; j< l->arr[i]->secondaries->sz; j++)
        {
            gc->prog->declarations = append_declaration_list(
                gc->prog->declarations,
                new_DeclCardinalityVariable(
                    rdup_origsymbol(l->arr[i]->secondaries->arr[j]->name),
                    0,
                    new_Pragma_list()));
        }
    }
}

tmstring int2tmstring(const int i)
{
    char buf[256];
    sprintf(buf, "%d", i);
    return new_tmstring(buf);
}

expression build_numberOfProcessors_expression(global_context gc)
{
  if (enableSymbolicNumberOfProcessors)
    {
      return new_ExprName(add_origsymbol("numberOfProcessors"));
    }
  else
    {
      return new_ExprInt(gc->numberOfProcessors);
    }
}

expression build_currentProcessor_expression(global_context /*gc*/)
{
    return new_ExprFunctionCall(
	new_ExprName(add_origsymbol("thisProcessor")),
	new_expression_list());
}

bool stmtHasOwner(statement s)
{
    assert (s->on == OwnerExpr_listNIL ||
	    s->on->sz < 2);
    return (s->on != OwnerExpr_listNIL &&
	    s->on->sz == 1);
}

bool lhsHasOwner(SmtAssign sa)
{
    if (sa->lhs->tag != TAGLocWrapper)
	return false;
    LocWrapper lw = to_LocWrapper(sa->lhs);
    assert (lw->on == OwnerExpr_listNIL ||
	    lw->on->sz < 2);
    return (lw->on != OwnerExpr_listNIL &&
	    lw->on->sz == 1);
}

bool rhsHasOwner(SmtAssign sa)
{
    if (sa->rhs->tag != TAGExprWrapper)
	return false;
    ExprWrapper ew = to_ExprWrapper(sa->rhs);
    assert (ew->on == OwnerExpr_listNIL ||
	    ew->on->sz < 2);
    return (ew->on != OwnerExpr_listNIL &&
	    ew->on->sz == 1);
}

bool is2owned(SmtAssign sa)
{
    bool stst = stmtHasOwner(sa);
//    bool ltst = lhsHasOwner(sa);
    bool rtst = rhsHasOwner(sa);

//    fprintf(stderr,"stst:%d, sa->on->sz:%d\n",stst, stst?sa->on->sz:0);
//    fprintf(stderr,"ltst:%d\n",ltst);
//    fprintf(stderr,"rtst:%d\n",rtst);
    assert (!(rtst && !stst));
    return (stst && rtst);
}

ArrayProcDecl get_processor_decl(global_context gc)
{
    assert(gc->processors != ProcessorDecl_listNIL);
    assert(gc->processors->sz == 1);
    assert(gc->processors->arr[0]->tag == TAGArrayProcDecl);
    return to_ArrayProcDecl(gc->processors->arr[0]);
}

/*
  Produce a processor list from expression proc with the same number of dimensions
  as the distribution in on.
  on is an read-only expression
  proc can be removed after transformation
  */
expression_list build_procList(expression proc, OwnerExpr on, global_context gc)
{
    assert(on != OwnerExprNIL);
    assert(proc != expressionNIL);
    expression_list ret = new_expression_list();
    switch (on->tag)
    {
	case TAGNewOwnLocation:
	{
	    NewOwnLocation nol = to_NewOwnLocation(on);
	    ArrayProcDecl apd = get_processor_decl(gc);
	    assert(nol->proc->tag == TAGExprName);
	    assert (nol->distr->sz == 1 || nol->distr->sz == apd->sizes->sz);
	    for (unsigned i=0; i<nol->distr->sz; i++)
	    {
		expression tmpRet = rdup_expression(proc);
		if (nol->distr->sz != 1)
		{
		    int divFactor = 1;
		    for (unsigned j=0; j<i; j++)
			divFactor *= apd->sizes->arr[j];
		    int modFactor = divFactor * apd->sizes->arr[i];
		    tmpRet = new_ExprBinop(
			tmpRet,
			BINOP_MOD,
			new_ExprInt(modFactor));
		    if (divFactor != 1)
		    {
			tmpRet = new_ExprBinop(
			    tmpRet,
			    BINOP_DIVIDE,
			    new_ExprInt(divFactor));
		    }
		}
		ret = append_expression_list(ret, tmpRet);
	    }
	    rfre_expression(proc);
	    break;
	}
	case TAGNewOwnReplicated:
	    ret = append_expression_list(ret, proc);
	    break;
	case TAGNewOwnDontCare:
	case TAGNewOwnMap:
	    internal_error("service: cannot create procList using distribution\n");
	    break;
    }
    return ret;
}

extern bool isBasicType(expression e, global_context gc)
{
    bool res = false;
    type t = derive_type_expression(gc->prog->declarations, 0, e);
    assert (t != typeNIL);
    type t2 = t;
    while (t2->tag == TAGTypeMap)
    {
	t2 = to_TypeMap(t2)->t;
	assert (t2 != typeNIL);
    }
    switch (t2->tag)
    {
	case TAGTypeBase:
	    res = true;
	    break;
	case TAGTypeMap:
	case TAGTypeShape:
	case TAGTypeUnsizedArray:
	case TAGTypePointer:
	case TAGTypeNeutralPointer:
	case TAGTypeArray:
	case TAGTypeRecord:
	case TAGTypeNamedRecord:
	case TAGTypeProcedure:
	case TAGTypeFunction:
	case TAGTypePragmas:
	    break;
    }
    rfre_type(t);
    return res;
}
