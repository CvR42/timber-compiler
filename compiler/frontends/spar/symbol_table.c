/* File: symbol_table.c
 *
 * Handling of the symbol table.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <tmc.h>

#include "defs.h"
#include "tmadmin.h"
#include "global.h"
#include "error.h"
#include "prettyprint.h"
#include "symbol_table.h"
#include "typederive.h"
#include "typename.h"
#include "doimports.h"
#include "service.h"
#include "collect.h"
#include "dump.h"

/* Given a symbol table and a tmsymbol 'name', search the entry for
 * that name. Return false if it cannot be found. Return true if
 * it can be found. Assign the position in the table where the entry
 * is or should be to '*pos'.
 *
 * Since this function was very high in the profile, the 'data' field
 * of each symbol is abused to store a guess of the position of that
 * symbol in the symboltable.
 *
 * Also, the function is inlined.
 */
static bool search_Entry_list( const_Entry_list symtab, const tmsymbol name, unsigned int *pos )
{
    unsigned int oldix = reinterpret_cast<unsigned int> (name->data);
    if( oldix<symtab->sz && symtab->arr[oldix]->name == name ){
	*pos = oldix;
	return true;
    }
    for( unsigned int ix = 0; ix<symtab->sz; ix++ ){
	const_Entry e = symtab->arr[ix];

	if( name == e->name ){
	    *pos = ix;
	    name->data = reinterpret_cast< void * >( ix );
	    return true;
	}
    }
    *pos = symtab->sz;
    return false;
}

Entry_list add_symtab( Entry_list symtab, Entry e )
{
    unsigned int pos;

    if( search_Entry_list( symtab, e->name, &pos ) ){
	Entry olde = symtab->arr[pos];

	if(
	    olde->tag != TAGForwardFunctionEntry &&
	    olde->tag != TAGForwardFieldEntry
	){
	    internal_error( "duplicate symbol table entry for '%s'", e->name->name );
	}
	if( is_MethodEntry( e ) != is_MethodEntry( olde ) ){
	    internal_error( "method/variable confusion for '%s'", e->name->name );
	}
	else {
	    if( is_MethodEntry( e ) ){
		if( to_MethodEntry(olde)->isvirtuallyused ){
		    to_MethodEntry(e)->isvirtuallyused = true;
		}
		to_MethodEntry(e)->directusecount += to_MethodEntry(olde)->directusecount;
	    }
	}
	if( trace_symtab ){
	    fprintf(
		stderr,
		"Replacing old entry from symtab: '%s'\n",
		olde->name->name
	    );
	}
	symtab = delete_Entry_list( symtab, pos );
	symtab = append_Entry_list( symtab, e );
    }
    else {
	symtab = append_Entry_list( symtab, e );
    }
    if( trace_symtab ){
	dump_Entry( stderr, e, "Add to symtab:" );
    }
    return symtab;
}

/* Given a tmsymbol 'name', return the entry for that name.
 * Since the symbol table is supposed to describe every identifier
 * in the program, the function can (in principle) not fail.
 * If it does, it is treated as an internal error.
 */
Entry search_Entry( const_Entry_list symtab, const tmsymbol name )
{
    unsigned int ix;

    if( !search_Entry_list( symtab, name, &ix ) ){
	return EntryNIL;
    }
    return symtab->arr[ix];
}

// The same as above, but for an origsymbol
Entry search_Entry( const_Entry_list symtab, const_origsymbol name )
{
    return search_Entry( symtab, name->sym );
}


/* Given a tmsymbol 'name', return the entry for that name.
 * Since the symbol table is supposed to describe every identifier
 * in the program, the function can (in principle) not fail.
 * If it does, it is treated as an internal error.
 */
Entry lookup_Entry( const_Entry_list symtab, const tmsymbol name )
{
    if( name == 0 ){
	return EntryNIL;
    }
    Entry e = search_Entry( symtab, name );
    if( e == EntryNIL ){
	internal_error( "Identifier '%s' is not in symbol table", name->name );
    }
    return e;
}

/* Given a oirgsymbol 'name', return the entry for that name.
 * Since the symbol table is supposed to describe every identifier
 * in the program, the function can (in principle) not fail.
 * If it does, it is treated as an internal error.
 */
Entry lookup_Entry( const_Entry_list symtab, const_origsymbol name )
{
    if( name == origsymbolNIL || name->sym == tmsymbolNIL ){
	return EntryNIL;
    }
    Entry e = search_Entry( symtab, name->sym );
    if( e == EntryNIL ){
	origsymbol_internal_error( name, "Identifier is not in symbol table" );
    }
    return e;
}

/* Given a tmsymbol 'name', return the entry for that name.
 * The symbol should refer to a VariableEntry. If not, complain.
 */
VariableEntry search_VariableEntry( const_Entry_list symtab, const tmsymbol name )
{
    if( name == 0 ){
	return VariableEntryNIL;
    }
    Entry e = search_Entry( symtab, name );
    if( e == EntryNIL ){
	return VariableEntryNIL;
    }
    if( !is_VariableEntry(e) ){
	internal_error( "Identifier '%s' is not a variable", name->name );
    }
    return to_VariableEntry(e);
}

/* Given a tmsymbol 'name', return the entry for that name.
 * Since the symbol table is supposed to describe every identifier
 * in the program, the function can (in principle) not fail.
 * If it does, it is treated as an internal error.
 *
 * The symbol should refer to a VariableEntry. If not, complain.
 */
VariableEntry lookup_VariableEntry( const_Entry_list symtab, const tmsymbol name )
{
    if( name == 0 ){
	return VariableEntryNIL;
    }
    VariableEntry e = search_VariableEntry( symtab, name );
    if( e == EntryNIL ){
	internal_error( "Identifier '%s' is not in symbol table", name->name );
    }
    return e;
}

/* Given a tmsymbol 'name', return the entry for that name.
 * Since the symbol table is supposed to describe every identifier
 * in the program, the function can (in principle) not fail.
 * If it does, it is treated as an internal error.
 *
 * The symbol should refer to a MethodEntry. If not, complain.
 */
MethodEntry lookup_MethodEntry( const_Entry_list symtab, const tmsymbol name )
{
    if( name == 0 ){
	return MethodEntryNIL;
    }
    Entry e = search_Entry( symtab, name );
    if( e == EntryNIL ){
	internal_error( "Identifier '%s' is not in symbol table", name->name );
    }
    if( !is_MethodEntry(e) ){
	internal_error( "Identifier '%s' is not a method", name->name );
    }
    return to_MethodEntry(e);
}

/* Given a list of field mappings 'ml', a symbol 'nm' and a pointer to an
 * index 'pos', return false if no field with original name 'nm' occurs
 * in the list. Return true if it does occur, and set '*pos' to the index
 * of this field in the list.
 */
static bool search_FieldMapping( const_FieldMapping_list ml, tmsymbol nm, unsigned int *pos )
{
    for( unsigned int ix=0; ix<ml->sz; ix++ ){
	if( ml->arr[ix]->realname->name == nm ){
	    *pos = ix;
	    return true;
	}
    }
    return false;
}

/* Given a tmsymbol 'name', return false if the type cannot be found,
 * or return true if it can be found, and assign the index in the list
 * to '*pos'.
 */
bool search_TypeEntry_pos( const_TypeEntry_list typelist, const tmsymbol name, unsigned int *pos )
{
    if( typelist == TypeEntry_listNIL ){
	return false;
    }
    for( unsigned int ix = 0; ix<typelist->sz; ix++ ){
	TypeEntry e = typelist->arr[ix];

	if( name == e->name->sym ){
	    *pos = ix;
	    return true;
	}
    }
    return false;
}

/* Given a tmsymbol 'name', return the entry for that name.  */
TypeEntry search_TypeEntry( const_TypeEntry_list typelist, const tmsymbol name )
{
    if( typelist == TypeEntry_listNIL ){
	return TypeEntryNIL;
    }
    for( unsigned int ix = 0; ix<typelist->sz; ix++ ){
	TypeEntry e = typelist->arr[ix];

	if( name == e->name->sym ){
	    return e;
	}
    }
    return TypeEntryNIL;
}

TypeEntry search_TypeEntry( const_TypeEntry_list typelist, const_origsymbol s )
{
    if( s == origsymbolNIL ){
	return TypeEntryNIL;
    }
    return search_TypeEntry( typelist, s->sym );
}

static TypeEntry search_TypeEntry( const_TypeEntry_list typelist, const_type t )
{
    assert( t->tag == TAGObjectType );
    return search_TypeEntry( typelist, to_const_ObjectType(t)->name );
}

/* Given a tmsymbol 'name', return the entry for that name.
 * Since the symbol table is supposed to describe every identifier
 * in the program, the function can (in principle) not fail.
 * If it does, it is treated as an internal error.
 */
TypeEntry lookup_TypeEntry( const_TypeEntry_list typelist, const tmsymbol name )
{
    TypeEntry e = search_TypeEntry( typelist, name );

    if( e == TypeEntryNIL ){
	internal_error( "Identifier '%s' is not in type list", name->name );
    }
    return e;
}

/* Given a origsymbol 'name', return the entry for that name.
 * Since the symbol table is supposed to describe every identifier
 * in the program, the function can (in principle) not fail.
 * If it does, it is treated as an internal error.
 */
TypeEntry lookup_TypeEntry( const_TypeEntry_list typelist, const_origsymbol name )
{
    if( name == origsymbolNIL ){
	return TypeEntryNIL;
    }
    TypeEntry e = search_TypeEntry( typelist, name->sym );

    if( e == TypeEntryNIL ){
	origsymbol_internal_error( name, "Identifier is not in type list" );
    }
    return e;
}

// Given an object type 't', return the entry for that type.
TypeEntry lookup_TypeEntry( const_TypeEntry_list typelist, const_type t )
{
    TypeEntry res = TypeEntryNIL;

    switch( t->tag ){
	case TAGArrayType:
	case TAGExceptionVariableType:
	case TAGFunctionType:
	case TAGGCRefLinkType:
	case TAGGCTopRefLinkType:
	case TAGGenericObjectType:
	case TAGNullType:
	case TAGPrimArrayType:
	case TAGPrimitiveType:
	case TAGTupleType:
	case TAGTypeType:
	case TAGVectorType:
	case TAGVoidType:
	    internal_error( "This type has no entry" );
	    break;

	case TAGTypeOfIf:
	    internal_error( "TypeOfIf should have been rewritten" );
	    break;

	case TAGTypeOf:
	    internal_error( "TypeOf should have been rewritten" );
	    break;

	case TAGPragmaType:
	    res = lookup_TypeEntry( typelist, to_const_PragmaType(t)->t );
	    break;

	case TAGObjectType:
	    res = lookup_TypeEntry( typelist, to_const_ObjectType(t)->name );
	    break;

    }
    return res;
}

static TypeEntry_list register_subclass( TypeEntry_list typelist, tmsymbol super, tmsymbol sub )
{
    TypeEntry e = lookup_TypeEntry( typelist, super );
    if( trace_typelist|trace_usage ){
	fprintf( stderr, "Register '%s' as subclass of '%s'\n", sub->name, super->name );
    }
    if( e == TypeEntryNIL ){
	if( trace_typelist ){
	    fprintf( stderr, "Failed - class not found\n" );
	}
	return typelist;
    }
    e->subclasses = append_tmsymbol_list( e->subclasses, sub );
    return typelist;
}

TypeEntry_list register_subclass( TypeEntry_list typelist, const_origsymbol super, tmsymbol sub )
{
    return register_subclass( typelist, super->sym, sub );
}

TypeEntry_list register_TypeEntry( TypeEntry_list typelist, TypeEntry e )
{
    unsigned int pos;

    if( trace_typelist ){
	const char *nm = (char *) NULL;

	switch( e->tag ){
	    case TAGInterfaceEntry:	nm = "interface";	break;
	    case TAGClassEntry:		nm = "class";		break;
	    case TAGPackageEntry:	nm = "package";		break;
	    case TAGForwardObjectEntry:	nm = "forward type";	break;
	}
	fprintf( stderr, "Add to type list: %s '%s' ", nm, e->name->sym->name );
	if( e->package == tmsymbolNIL ){
	    fputs( "in unnamed package", stderr );
	}
	else {
	    fprintf( stderr, "in package '%s'", e->package->name );
	}
	if( e->enclosingtype == tmsymbolNIL ){
	    fputs( "; no enclosing type", stderr );
	}
	else {
	    fprintf( stderr, "; enclosing type '%s'", e->enclosingtype->name );
	}
	fputs( "\n", stderr );
    }
    if( search_TypeEntry_pos( typelist, e->name->sym, &pos ) ){
	TypeEntry old_e = typelist->arr[pos];
	e->subclasses = concat_tmsymbol_list( e->subclasses, old_e->subclasses );
	e->usecount += old_e->usecount;
	old_e->subclasses = tmsymbol_listNIL;
	typelist = delete_TypeEntry_list( typelist, pos );
	if( trace_typelist ){
	    fprintf( stderr, "Deleted old entry for '%s' from type list\n", e->name->sym->name );
	}
    }
    typelist = append_TypeEntry_list( typelist, e );
    return typelist;
}

// Forward declaration.
bool is_subclass( const_TypeEntry_list typelist, tmsymbol sub, const_TypeEntry super );

/* Given a tmsymbol 'super' and a tmsymbol 'sub', return true
 * iff 'sub' is the same class or a subclass of 'super'.
 */
bool is_subclass( const_TypeEntry_list typelist, const_TypeEntry sub, const_TypeEntry super )
{
    if( super == sub ){
	// That's the easy part.
	return true;
    }
    if( super->tag != TAGClassEntry || sub->tag != TAGClassEntry ){
	// This may in fact be an ill-posed question, but don't bother
	// to complain.
	return false;
    }
    const_ClassEntry ce_sub = to_const_ClassEntry( sub );
    const_type sub_super = ce_sub->super;
    if( sub_super == typeNIL ){
	// Supposedly this is java.lang.Object. Since we already know
	// super != sub, we must conclude the answer is no.
	return false;
    }
    assert( sub_super->tag == TAGObjectType );
    return is_subclass( typelist, to_const_ObjectType(sub_super)->name->sym, super );
}

/* Given a tmsymbol 'super' and a tmsymbol 'sub', return true
 * iff 'sub' is the same class or a subclass of 'super'.
 */
bool is_subclass( const_TypeEntry_list typelist, tmsymbol sub, const_TypeEntry super )
{
    TypeEntry te_sub = search_TypeEntry( typelist, sub );
    if( te_sub == TypeEntryNIL ){
	internal_error( "Cannot find entry for type '%s'", sub->name );
    }
    return is_subclass( typelist, te_sub, super );
}

/* Given a tmsymbol 'super' and a tmsymbol 'sub', return true
 * iff 'sub' is the same class or a subclass of 'super'.
 */
bool is_subclass( const_TypeEntry_list typelist, tmsymbol sub, tmsymbol super )
{
    if( super == sub ){
	// That's the easy part.
	return true;
    }
    TypeEntry te_super = search_TypeEntry( typelist, super );
    if( te_super == TypeEntryNIL ){
	// Since the supposed superclass is not even known, it cannot possibly
	// be the superclass, so we can confidently say 'no' here.
	return false;
    }
    assert( te_super != TypeEntryNIL );
    return is_subclass( typelist, sub, te_super );
}

// Given a list of type entries and two types, return true iff the
// first type is a subclass of the second type.
bool is_subclass( const_TypeEntry_list typelist, const_type sub, tmsymbol super )
{
    if( sub->tag != TAGObjectType ){
	return false;
    }
    const_origsymbol nm = to_const_ObjectType( sub )->name;
    if( nm == origsymbolNIL ){
	return false;
    }
    return is_subclass( typelist, nm->sym, super );
}

// Given a list of type entries and two types, return true iff the
// first type is a subclass of the second type.
bool is_subclass( const_TypeEntry_list typelist, const_type sub, const_type super )
{
    if( sub->tag != TAGObjectType || super->tag != TAGObjectType ){
	return false;
    }
    return is_subclass(
	typelist,
	to_const_ObjectType( sub )->name->sym,
	to_const_ObjectType( super )->name->sym
    );
}

// Forward declaration.
static bool implements_interface( const_TypeEntry_list typelist, const_origsymbol nm, tmsymbol inf );

// Given a list of known types, a type and the name of an
// interface, return true iff the type implements the interface.
bool implements_interface( const_TypeEntry_list typelist, const_type t, tmsymbol inf )
{
    switch( t->tag ){
	case TAGArrayType:
	case TAGExceptionVariableType:
	case TAGFunctionType:
	case TAGGCRefLinkType:
	case TAGGCTopRefLinkType:
	case TAGGenericObjectType:
	case TAGNullType:
	case TAGPrimArrayType:
	case TAGPrimitiveType:
	case TAGTupleType:
	case TAGTypeOf:
	case TAGTypeOfIf:
	case TAGTypeType:
	case TAGVectorType:
	case TAGVoidType:
	    break;

	case TAGPragmaType:
	    return implements_interface( typelist, to_const_PragmaType(t)->t, inf );

	case TAGObjectType:
	    return implements_interface( typelist, to_const_ObjectType(t)->name, inf );
    }
    return false;
}

bool implements_interface( const_TypeEntry_list typelist, const_type t, const char *inf )
{
    return implements_interface( typelist, t, add_tmsymbol( inf ) );
}

// Given a list of known types, the name of a type and the name of an
// interface, return true iff the type implements the interface.
static bool implements_interface( const_TypeEntry_list typelist, const_origsymbol nm, tmsymbol inf )
{
    if( nm == origsymbolNIL || nm->sym == tmsymbolNIL ){
	return false;
    }
    if( nm->sym == inf ){
	return true;
    }
    const_TypeEntry te = search_TypeEntry( typelist, nm );
    if( te == TypeEntryNIL ){
	// This is an error somewhere, but we're not in a position to
	// generate a sensible error message.
	return false;
    }
    if( !is_ObjectTypeEntry( te ) ){
	origsymbol_internal_error( nm, "Not an object type" );
    }
    const_ObjectTypeEntry ote = to_const_ObjectTypeEntry( te );

    const_type_list interfaces = ote->interfaces;
    if( interfaces == type_listNIL ){
	return false;
    }
    for( unsigned int ix=0; ix<interfaces->sz; ix++ ){
	const_type t = interfaces->arr[ix];

	if( implements_interface( typelist, t, inf ) ){
	    return true;
	}
    }
    if( ote->tag == TAGClassEntry ){
	const_type super = to_const_ClassEntry(ote)->super;

	if(
	    super != typeNIL &&
	    implements_interface( typelist, super, inf )
	){
	    return true;
	}
    }
    return false;
}

// Given a list of known types, the name of a type and the name of an
// interface, return true iff the type implements the interface.
bool implements_interface( const_TypeEntry_list typelist, origsymbol nm, const char *inf )
{
    return implements_interface( typelist, nm, add_tmsymbol( inf ) );
}

// Given a list of known types, the name of a type and the name of an
// interface, return true iff the type implements the interface.
bool implements_interface( const_TypeEntry_list typelist, const_type_list types, const_origsymbol inf )
{
    for( unsigned int ix=0; ix<types->sz; ix++ ){
	if( implements_interface( typelist, types->arr[ix], inf->sym ) ){
	    return true;
	}
    }
    return false;
}

#if 0
// Forward declaration
static bool is_enclosing_class(
 const_TypeEntry_list typelist,
 tmsymbol inner,
 const_TypeEntry outer
);

// Given a list of type entries and two type entries, return true iff
// the 'outer' entry denotes an outer class of the 'inner' entry.
static bool is_enclosing_class(
 const_TypeEntry_list typelist,
 const_TypeEntry inner,
 const_TypeEntry outer
)
{
    if( outer == inner ){
	return true;
    }
    if( !is_ConcreteTypeEntry( inner ) ){
	return false;
    }
    tmsymbol enclosingtype = to_const_ConcreteTypeEntry(inner)->enclosingtype;
    if( enclosingtype == tmsymbolNIL ){
	return false;
    }
    return is_enclosing_class( typelist, enclosingtype, outer );
}

// Forward declaration
static bool is_enclosing_class(
 const_TypeEntry_list typelist,
 tmsymbol inner,
 const_TypeEntry outer
)
{
    const_TypeEntry inner_entry = lookup_TypeEntry( typelist, inner );
    if( inner_entry == TypeEntryNIL ){
	return false;
    }
    return is_enclosing_class( typelist, inner_entry, outer );
}
#endif

// Forward declaration.
static const_TypeEntry get_outer_class( const_TypeEntry_list typelist, const_TypeEntry e );

// Given a type entry 'te', return the type entry of the outer class
// of this type.
static const_TypeEntry get_outer_class( const_TypeEntry_list typelist, tmsymbol tnm )
{
    const_TypeEntry te = lookup_TypeEntry( typelist, tnm );
    if( te == TypeEntryNIL ){
	return TypeEntryNIL;
    }
    return get_outer_class( typelist, te );
}

// Given a type entry 'te', return the type entry of the outer class
// of this type.
static const_TypeEntry get_outer_class( const_TypeEntry_list typelist, const_TypeEntry e )
{
    if( !has_any_flag( e->flags, ACC_MEMBER_OBJECT|ACC_LOCAL_OBJECT ) ){
	return e;
    }
    assert( is_ConcreteTypeEntry( e ) );
    tmsymbol enclosingtype = to_const_ConcreteTypeEntry(e)->enclosingtype;
    assert( enclosingtype != tmsymbolNIL );
    return get_outer_class( typelist, enclosingtype );

}

/* Given sufficient information to determine this, return true if
 * the given type member can be accessed.
 */
bool is_valid_access(
 const_TypeEntry_list typelist,
 modflags flags,
 tmsymbol in_where,
 tmsymbol from_where
)
{
    if( from_where == tmsymbolNIL || in_where == tmsymbolNIL ){
	return true;
    }
    if( in_where == from_where ){
	// All access rights allow access from the same type.
	// Cut short this very common case.
	return true;
    }
    const_TypeEntry in_entry = lookup_TypeEntry( typelist, in_where );
    const_TypeEntry from_entry = lookup_TypeEntry( typelist, from_where );

    if( in_entry == TypeEntryNIL || from_entry == TypeEntryNIL ){
	// Don't bother to complain if there is an other error.
	return true;
    }
    const_TypeEntry in_outer_entry = get_outer_class( typelist, in_entry );
    const_TypeEntry from_outer_entry = get_outer_class( typelist, from_entry );
    if( in_outer_entry != NULL && in_outer_entry == from_outer_entry ){
	return true;
    }
    if( has_any_flag( flags, ACC_PRIVATE ) ){
	return false;
    }
    if( has_any_flag( flags, ACC_PUBLIC ) ){
	return true;
    }

    // At this point, access is either protected or default.

    /* protected or default access in the same package is allowed. */
    if( in_entry->package == from_entry->package ){
	return true;
    }
    if( has_any_flag( flags, ACC_PROTECTED ) ){
	if( in_entry->tag != TAGClassEntry || from_entry->tag != TAGClassEntry ){
	    return false;
	}
	return is_subclass( typelist, from_where, in_where );
    }
    return false;
}

// Forward declaration
static type lookup_vartype( const_Entry_list symtab, const tmsymbol name );

/* Given a name 'name' that is supposed to be a variable,
 * return a copy of the type of that variable.
 */
type lookup_vartype( const_Entry_list symtab, const_origsymbol name )
{
    if( name == origsymbolNIL ){
	return typeNIL;
    }
    return lookup_vartype( symtab, name->sym );
}

/* Given a name 'name' that is supposed to be a variable,
 * return a copy of the type of that variable.
 */
type lookup_vartype( const_Entry e )
{
    if( e == EntryNIL ){
	return typeNIL;
    }
    switch( e->tag ){
	case TAGGCLinkEntry:
	    return new_GCRefLinkType();

	case TAGGCTopLinkEntry:
	    return new_GCTopRefLinkType();

	case TAGCardinalityVariableEntry:
	    return new_PrimitiveType( BT_INT );

        case TAGGlobalVariableEntry:
	    return rdup_type( to_const_GlobalVariableEntry(e)->t );

        case TAGLocalVariableEntry:
	    return rdup_type( to_const_LocalVariableEntry(e)->t );

        case TAGForwardFieldEntry:
	    return rdup_type( to_const_ForwardFieldEntry(e)->t );

        case TAGFieldEntry:
	    return rdup_type( to_const_FieldEntry(e)->t );

        case TAGFormalParameterEntry:
	    return rdup_type( to_const_FormalParameterEntry(e)->t );

        case TAGFunctionEntry:
	{
	    const_FunctionEntry fe = to_const_FunctionEntry(e);

	    return new_FunctionType(
		derive_type_FormalParameter_list( fe->thisparameters ),
		derive_type_FormalParameter_list( fe->parameters ),
		rdup_type( fe->t )
	    );
	}

        case TAGForwardFunctionEntry:
	    return new_FunctionType(
		derive_type_FormalParameter_list( to_const_ForwardFunctionEntry(e)->thisparameters ),
		derive_type_FormalParameter_list( to_const_ForwardFunctionEntry(e)->parameters ),
		rdup_type( to_const_ForwardFunctionEntry(e)->t )
	    );

        case TAGExternalFunctionEntry:
	    return new_FunctionType(
		derive_type_FormalParameter_list( to_const_ExternalFunctionEntry(e)->thisparameters ),
		derive_type_FormalParameter_list( to_const_ExternalFunctionEntry(e)->parameters ),
		rdup_type( to_const_ExternalFunctionEntry(e)->t )
	    );

        case TAGAbstractFunctionEntry:
	    return new_FunctionType(
		derive_type_FormalParameter_list( to_const_AbstractFunctionEntry(e)->thisparameters ),
		derive_type_FormalParameter_list( to_const_AbstractFunctionEntry(e)->parameters ),
		rdup_type( to_const_AbstractFunctionEntry(e)->t )
	    );
    }
    return typeNIL;
}

/* Given a name 'name' that is supposed to be a variable,
 * return a copy of the type of that variable.
 */
static type lookup_vartype( const_Entry_list symtab, const tmsymbol name )
{
    if( name == tmsymbolNIL ){
	return typeNIL;
    }
    const_Entry d = search_Entry( symtab, name );
    return lookup_vartype( d );
}

// Forward declaration
static tmsymbol_list lookup_object_all_fields_recursion(
    tmsymbol_list fields,
    const_TypeEntry_list typelist,
    type in_where,
    want_static ws
);

/* Given a symbol table and an object name 'obj', return the list of
 * fields of this object, in 'depth first' order.
 */
static tmsymbol_list lookup_object_all_fields_recursion(
    tmsymbol_list fields,
    const_TypeEntry_list typelist,
    tmsymbol in_where,
    want_static ws
)
{
    FieldMapping_list ml = FieldMapping_listNIL;
    unsigned int ix;

    if( in_where == tmsymbolNIL ){
	return fields;
    }
    TypeEntry t = search_TypeEntry( typelist, in_where );
    if( t == TypeEntryNIL ){
	internal_error( "Cannot determine fields of type '%s'", in_where->name );
    }
    switch( t->tag ){
	case TAGClassEntry:
	{
	    type super = to_ClassEntry(t)->super;

	    if( super != typeNIL ){
		fields = lookup_object_all_fields_recursion(
		    fields,
		    typelist,
		    super,
		    ws
		);
	    }
	    ml = to_ClassEntry( t )->fields;
	    break;
	}

	case TAGInterfaceEntry:
	{
	    unsigned int six;
	    type_list supers = to_InterfaceEntry(t)->interfaces;

	    for( six=0; six<supers->sz; six++ ){
		fields = lookup_object_all_fields_recursion(
		    fields,
		    typelist,
		    supers->arr[six],
		    ws
		);
	    }
	    ml = to_InterfaceEntry( t )->fields;
	    break;
	}

	case TAGForwardObjectEntry:
	    internal_error( "Forward object entry should have been resolved" );
	    break;

	case TAGPackageEntry:
	    /* A package doesn't inherit. */
	    ml = to_PackageEntry( t )->fields;
	    break;

    }
    if( ml != FieldMapping_listNIL ){
	fields = setroom_tmsymbol_list( fields, fields->sz+ml->sz );
	for( ix=0; ix<ml->sz; ix++ ){
	    FieldMapping m = ml->arr[ix];

	    if( ws == WANT_STATIC || (m->realname->flags & ACC_STATIC) == 0 ){
		fields = append_tmsymbol_list( fields, m->mangledname );
	    }
	}
    }
    return fields;
}

static tmsymbol_list lookup_object_all_fields_recursion(
    tmsymbol_list fields,
    const_TypeEntry_list typelist,
    type in_where,
    want_static ws
)
{
    if( in_where == typeNIL ){
	return fields;
    }
    assert( in_where->tag == TAGObjectType );
    return lookup_object_all_fields_recursion( fields, typelist, to_ObjectType(in_where)->name->sym, ws );
}

/* Given a symbol table and an object name 'obj', return the list of
 * fields of this object, in 'depth first' order.
 */
tmsymbol_list lookup_object_all_fields(
 const_TypeEntry_list typelist,
 tmsymbol obj,
 want_static st
)
{
    return lookup_object_all_fields_recursion(
	new_tmsymbol_list(),
	typelist,
	obj,
	st
    );
}

// Forward declaration
static const_FieldMapping bind_Field_to_Mapping(
 SparProgram *prog,
 const_Entry_list symtab,
 const_TypeEntry_list typelist,
 const_type in_where,
 tmsymbol from_where,
 const_origsymbol field
);

/* Given a symbol table, a type and the name of a field, return the
 * mangled name of the field.
 */
const_FieldMapping bind_Field_to_Mapping(
 SparProgram *prog,
 const_Entry_list symtab,
 const_TypeEntry_list typelist,
 tmsymbol in_where,
 tmsymbol from_where,
 const_origsymbol field
)
{
    unsigned int pos;
    const_FieldMapping res = FieldMappingNIL;
    bool ok;

    if( in_where == tmsymbolNIL ){
	return FieldMappingNIL;
    }
    ok = compile_type( prog, in_where );
    if( !ok ){
	return FieldMappingNIL;
    }
    ok = compile_type( prog, from_where );
    if( !ok ){
	return FieldMappingNIL;
    }
    const_TypeEntry t = lookup_TypeEntry( typelist, in_where );
    if( t == TypeEntryNIL ){
	return FieldMappingNIL;
    }
    if( !is_ConcreteTypeEntry( t ) ){
	internal_error( "Cannot bind field, forward object entry should have been resolved" );
    }
    const_ConcreteTypeEntry ct = to_const_ConcreteTypeEntry( t );
    const_FieldMapping_list ml = ct->fields;
    if( search_FieldMapping( ml, field->sym, &pos ) ){
	FieldMapping m = ml->arr[pos];

	if( !is_valid_access( typelist, m->realname->flags, in_where, from_where ) ){
	    origin_error(
		field->org,
		"Variable '%s' in '%s' is not accessible from '%s'.",
		field->sym->name,
		in_where->name,
		from_where->name
	    );
	}
	return m;
    }
    tmsymbol_list candidates = new_tmsymbol_list();
    switch( t->tag ){
	case TAGClassEntry:
	{
	    const_type super = to_const_ClassEntry( t )->super;

	    if( super != typeNIL ){
		const_FieldMapping res1 = bind_Field_to_Mapping(
		    prog,
		    symtab,
		    typelist,
		    super,
		    from_where,
		    field
		);
		if( res1 != FieldMappingNIL ){
		    candidates = append_tmsymbol_list( candidates, res1->mangledname );
		    res = res1;
		}
	    }
	    const_type_list interfaces = to_const_ClassEntry( t )->interfaces;
	    for( unsigned int ix=0; ix<interfaces->sz; ix++ ){
		const_FieldMapping res1 = bind_Field_to_Mapping(
		    prog,
		    symtab,
		    typelist,
		    interfaces->arr[ix],
		    from_where,
		    field
		);
		if(
		    res1 != FieldMappingNIL &&
		    !member_tmsymbol_list( candidates, res1->mangledname )
		){
		    res = res1;
		    candidates = append_tmsymbol_list( candidates, res1->mangledname );
		}
	    }
	    break;
	}

	case TAGInterfaceEntry:
	{
	    type_list interfaces = to_const_InterfaceEntry( t )->interfaces;
	    for( unsigned int ix=0; ix<interfaces->sz; ix++ ){
		const_FieldMapping res1 = bind_Field_to_Mapping(
		    prog,
		    symtab,
		    typelist,
		    interfaces->arr[ix],
		    from_where,
		    field
		);
		if( res1 != FieldMappingNIL && !member_tmsymbol_list( candidates, res1->mangledname ) ){
		    res = res1;
		    candidates = append_tmsymbol_list( candidates, res1->mangledname );
		}
	    }
	    break;
	}

	case TAGForwardObjectEntry:
	    internal_error( "Cannot bind field, forward object entry should have been resolved" );
	    break;

	case TAGPackageEntry:
	    break;

    }
    if( candidates->sz == 0 ){
	// Nothing found; try the enclosing class
	rfre_tmsymbol_list( candidates );
	return bind_Field_to_Mapping(
	    prog,
	    symtab,
	    typelist,
	    ct->enclosingtype,
	    from_where,
	    field
	);
    }
    if( candidates->sz != 1 ){
	origin_error(
	    field->org,
	    "Field reference '%s' in '%s' is ambiguous:",
	    field->sym->name,
	    from_where->name
	);
	for( unsigned int ix=0; ix<candidates->sz; ix++ ){
	    tmsymbol s = candidates->arr[ix];
	    Entry d = lookup_Entry( symtab, s );
	    if( d == EntryNIL || !is_VariableEntry( d ) ){
		error( "Candidate field: '%s' (but it's not in the symboltable!)", candidates->arr[ix]->name );
	    }
	    else {
		if( d->owner == tmsymbolNIL ){
		    origin_error(
			to_VariableEntry( d )->realname->org,
			"This is a candidate"
		    );
		}
		else {
		    origin_error(
			to_VariableEntry( d )->realname->org,
			"This is a candidate (in %s)",
			d->owner->name
		    );
		}
	    }
	}
	error( "You can resolve this by qualifying the name." );
    }
    rfre_tmsymbol_list( candidates );
    return res;
}

static const_FieldMapping bind_Field_to_Mapping(
 SparProgram *prog,
 const_Entry_list symtab,
 const_TypeEntry_list typelist,
 const_type in_where,
 tmsymbol from_where,
 const_origsymbol field
)
{
    if( in_where == typeNIL ){
	return FieldMappingNIL;
    }
    if( in_where->tag == TAGGenericObjectType ){
	return FieldMappingNIL;
    }
    assert( in_where->tag == TAGObjectType );
    return bind_Field_to_Mapping( prog, symtab, typelist, to_const_ObjectType(in_where)->name->sym, from_where, field );
}

/* Given a symbol table, a type and the name of a field, return the
 * mangled name of the field.
 */
VariableEntry bind_Field(
 SparProgram *prog,
 const_Entry_list symtab,
 const_TypeEntry_list typelist,
 tmsymbol in_where,
 tmsymbol from_where,
 const_origsymbol field
)
{
    const_FieldMapping m = bind_Field_to_Mapping( prog, symtab, typelist, in_where, from_where, field );

    if( m == FieldMappingNIL ){
	return VariableEntryNIL;
    }
    Entry d = lookup_Entry( symtab, m->mangledname );
    if( !is_VariableEntry( d ) ){
	internal_error( "Not a field entry" );
    }
    return to_VariableEntry( d );
}

/* Given two signatures 'target' and 'ref', return true if
 * 'ref' could be invoked from an invocation with signature 'target'.
 */
static bool is_applicable_method(
 const_Entry_list symtab,
 const_TypeEntry_list typelist,
 const_Signature target,
 const_Signature ref,
 tmsymbol in_where,
 tmsymbol from_where,
 bool *rejected
)
{
    if( target->name != ref->name ){
	if( showmethodbinding ){
	    tmstring targetnm = typename_Signature( target );
	    tmstring refnm = typename_Signature( ref );

	    fprintf(
		stderr,
		"%s is not applicable to %s: different method name\n",
		refnm,
		targetnm
	    );
	    rfre_tmstring( targetnm );
	    rfre_tmstring( refnm );
	}
	return false;
    }
    if( target->parameters->sz != ref->parameters->sz ){
	if( showmethodbinding ){
	    tmstring targetnm = typename_Signature( target );
	    tmstring refnm = typename_Signature( ref );

	    fprintf(
		stderr,
		"%s is not applicable to %s: different number of parameters\n",
		refnm,
		targetnm
	    );
	    rfre_tmstring( targetnm );
	    rfre_tmstring( refnm );
	}
	return false;
    }
    if( !is_valid_access( typelist, ref->flags, in_where, from_where ) ){
	if( showmethodbinding ){
	    tmstring targetnm = typename_Signature( target );
	    tmstring refnm = typename_Signature( ref );

	    fprintf(
		stderr,
		"%s is not applicable to %s: not accessible\n",
		refnm,
		targetnm
	    );
	    rfre_tmstring( targetnm );
	    rfre_tmstring( refnm );
	}
	*rejected = true;
	return false;
    }
    if( ! is_invocationequivalent_type_list(
	symtab,
	typelist,
	target->parameters,
	ref->parameters
    ) ){
	if( showmethodbinding ){
	    tmstring targetnm = typename_Signature( target );
	    tmstring refnm = typename_Signature( ref );

	    fprintf(
		stderr,
		"%s is not applicable to %s: not invocation equivalent\n",
		refnm,
		targetnm
	    );
	    rfre_tmstring( targetnm );
	    rfre_tmstring( refnm );
	}
	return false;
    }
    return true;
}

/* Given two type lists, return -1 if the first one is more specific
 * than the second one, return 1 if the second one is more specific than
 * the first one, and return 0 if no ordering is possible.
 * See JLS22 15.12.2.2 of the definition of 'more specific'.
 */
static int morespecific_type_list(
 const_Entry_list symtab,
 const_TypeEntry_list typelist,
 const type_list a,
 const type_list b
)
{
    unsigned int a_to_b_ok = 0;
    unsigned int b_to_a_ok = 0;

    assert( a->sz == b->sz );
    for( unsigned int ix=0; ix<a->sz; ix++ ){
	type ta = a->arr[ix];
	type tb = b->arr[ix];

        if( is_invocationequivalent_type( symtab, typelist, ta, tb ) ){
	    a_to_b_ok++;
	}
        if( is_invocationequivalent_type( symtab, typelist, tb, ta ) ){
	    b_to_a_ok++;
	}
    }
    if( a_to_b_ok == a->sz && b_to_a_ok == a->sz ){
	return 0;
    }
    if( a_to_b_ok == a->sz ){
	/* a is more specific than b */
	return -1;
    }
    if( b_to_a_ok == a->sz ){
	/* b is more specific than a */
	return 1;
    }
    return 0;
}

/* Given two signatures, return -1 if the first one is more specific
 * than the second one, return 1 if the second one is more specific than
 * the first one, and return 0 if no ordering is possible.
 */
static int morespecific_Signature(
 const_Entry_list symtab,
 const_TypeEntry_list typelist,
 const_Signature a,
 const_Signature b
)
{
    int res = morespecific_type_list( symtab, typelist, a->parameters, b->parameters );

    if( showmethodbinding ){
	tmstring sa = typename_Signature( a );
	tmstring sb = typename_Signature( b );
	const char *verdict;

	if( res == 1 ){
	    verdict = "less specific than";
	}
	else if( res == -1 ){
	    verdict = "more specific than";
	}
	else {
	    verdict = "as specific as";
	}
	fprintf( stderr, "%s is %s %s\n", sa, verdict, sb );
	rfre_tmstring( sa );
	rfre_tmstring( sb );
    }
    return res;
}

/* Given a list of method binding candidates and a new candidate, update the
 * list of candidates.
 *
 * The list may grow or shrink according to 15.11.2.2
 */
static MethodMapping_list update_candidate_list(
 const_Entry_list symtab,
 const_TypeEntry_list typelist,
 MethodMapping_list candidates,
 MethodMapping candidate
)
{
    bool reject = false;

    unsigned int ix = 0;
    while( ix<candidates->sz ){
	MethodMapping oldcan = candidates->arr[ix];

	int res = morespecific_Signature( symtab, typelist, oldcan->realname, candidate->realname );

	if( res == -1 ){
	    /* New candidate is less specific. */
	    reject = true;
	    ix++;
	}
	else if( res == 1 ){
	    /* New candidate is more specific, toss out the old one. */
	    if( showmethodbinding ){
		tmstring snm = typename_Signature( oldcan->realname );

		fprintf(
		    stderr,
		    "Method %s (=%s) has been overruled\n",
		    snm,
		    oldcan->mangledname->name
		);
		rfre_tmstring( snm );
	    }
	    candidates = delete_MethodMapping_list( candidates, ix );
	}
	else {
	    /* Equally specific. We'll have to see. */
	    ix++;
	}
    }
    if( showmethodbinding ){
	tmstring snm = typename_Signature( candidate->realname );

	fprintf(
	    stderr,
	    "Candidate method %s (=%s) has been %sed\n",
	    snm,
	    candidate->mangledname->name,
	    reject?"reject":"accept"
	);
	rfre_tmstring( snm );
    }
    if( !reject ){
	candidates = append_MethodMapping_list(
	    candidates,
	    rdup_MethodMapping( candidate )
	);
    }
    return candidates;
}

// Forward declaration
static MethodEntry search_Method(
 SparProgram *prog,
 const_Entry_list symtab,
 const_TypeEntry_list typelist,
 const_type where_class,		/* From which class is this invoked. */
 const tmsymbol invocation_class,	/* In which class is the method. */
 const_Signature s,			/* Desired signature of the method. */
 bool *is_static,
 bool *use_static_call,			// static call can/must be used?
 bool *rejected,
 const_origin org
);

/* Given a symbol table 'symtab', the name of the class the method is
 * expected to be in, the actual method signature 's', and a pointer
 * to a method name 'nmp', return the name of the method that
 * best matches the given signature. The matching process is
 * described in JLS2 15.12.2.2.
 *
 * If where_class is null, the global functions are searched.
 *
 * If two or more functions are equally specific, an error message is printed,
 * and an arbitrary one is returned.
 */
MethodEntry search_Method(
 SparProgram *prog,
 const_Entry_list symtab,
 const_TypeEntry_list typelist,
 const tmsymbol where_class,		/* From which class is this invoked. */
 const tmsymbol invocation_class,	/* In which class is the method. */
 const_Signature s,			/* Desired signature of the method. */
 bool *is_static,
 bool *use_static_call,			// static call can/must be used?
 bool *rejected,
 const_origin org
)
{
    MethodEntry res = MethodEntryNIL;
    bool ok;

    *use_static_call = false;
    if( where_class == tmsymbolNIL ){
	internal_error( "No class specified" );
    }
    if( showmethodbinding ){
	tmstring snm = typename_Signature( s );

	fprintf(
	    stderr,
	    "Looking for binding of method '%s' in '%s', invoked from '%s'\n",
	    snm,
	    where_class->name,
	    invocation_class==tmsymbolNIL?invocation_class->name:"(none)"
	);
	rfre_tmstring( snm );
    }
    ok = compile_type( prog, invocation_class );
    if( !ok ){
	if( showmethodbinding ){
	    // TODO: invocation_class can be null at this point. Why??
	    fprintf(
		stderr,
		"Could not compile '%s', giving up search\n",
		invocation_class==tmsymbolNIL?invocation_class->name:"(none)"
	    );
	}
	return MethodEntryNIL;
    }
    ok = compile_type( prog, where_class );
    if( !ok ){
	if( showmethodbinding ){
	    fprintf( stderr, "Could not compile '%s', giving up search\n", where_class->name );
	}
	return MethodEntryNIL;
    }
    MethodMapping_list candidates = new_MethodMapping_list();
    const_TypeEntry cl = lookup_TypeEntry( typelist, where_class );
    if( !is_ConcreteTypeEntry( cl ) ){
	internal_error( "Cannot bind method, forward object entry should have been resolved" );
    }
    const_ConcreteTypeEntry ccl = to_const_ConcreteTypeEntry( cl );
    MethodMapping_list methods = ccl->methods;
    {
	// Unnecessary block to shut up g++ about shadowing of 'ix'.
	for( unsigned int ix=0; ix<methods->sz; ix++ ){
	    MethodMapping method = methods->arr[ix];

	    if( showmethodbinding ){
		tmstring snm = typename_Signature( method->realname );

		fprintf( stderr, "Pondering '%s'\n", snm );
		rfre_tmstring( snm );
	    }
	    bool is_ok = is_applicable_method(
		symtab,
		typelist,
		s,
		method->realname,
		where_class,
		invocation_class,
		rejected
	    );
	    if( showmethodbinding ){
		fprintf( stderr, "%s\n", is_ok?"applicable":"not applicable" );
	    }
	    if( is_ok ){
		candidates = update_candidate_list(
		    symtab,
		    typelist,
		    candidates,
		    method
		);
	    }
	}
    }
    if( candidates->sz == 0 ){
	rfre_MethodMapping_list( candidates );
	switch( cl->tag ){
	    case TAGClassEntry:
	    {
		const_type super = to_const_ClassEntry( cl )->super;

		if( super != typeNIL ){
		    if( showmethodbinding ){
			fprintf(
			    stderr,
			    "no candidate method found in class '%s', try the superclass\n",
			    where_class->name
			);
		    }
		    res = search_Method(
			prog,
			symtab,
			typelist,
			super,
			invocation_class,
			s,
			is_static,
			use_static_call,
			rejected,
			org
		    );
		}
		break;
	    }

	    case TAGInterfaceEntry:
	    {
		const_type_list interfaces = to_const_InterfaceEntry( cl )->interfaces;
		if( interfaces != type_listNIL ){
		    if( showmethodbinding ){
			fprintf(
			    stderr,
			    "no candidate method found in interface '%s', try the %u superinterfaces\n",
			    where_class->name,
			    interfaces->sz
			);
		    }

		    for( unsigned int inf=0; inf<interfaces->sz; inf++ ){
			res = search_Method(
			    prog,
			    symtab,
			    typelist,
			    interfaces->arr[inf],
			    invocation_class,
			    s,
			    is_static,
			    use_static_call,
			    rejected,
			    org
			);
			// TODO: we should probably complain if
			// binding through different interfaces
			// causes different results.
			if( res != MethodEntryNIL ){
			    break;
			}
		    }
		}
		break;
	    }

	    case TAGForwardObjectEntry:
		internal_error( "Cannot bind method, forward object entry should have been resolved" );
		break;

	    case TAGPackageEntry:
		break;

	}
	if( res == MethodEntryNIL ){
	    if( ccl->enclosingtype != tmsymbolNIL ){
		if( showmethodbinding ){
		    fprintf(
			stderr,
			"no candidate method found in '%s', try the enclosing class '%s'\n",
			where_class->name,
			ccl->enclosingtype->name
		    );
		}
		res = search_Method(
		    prog,
		    symtab,
		    typelist,
		    ccl->enclosingtype,
		    invocation_class,
		    s,
		    is_static,
		    use_static_call,
		    rejected,
		    org
		);
	    }
	}
	if( showmethodbinding ){
	    tmstring snm = typename_Signature( s );

	    if( res == MethodEntryNIL ){
		fprintf(
		    stderr,
		    "Conclusion: no method in '%s' matches '%s'\n",
		    where_class->name,
		    snm
		);
	    }
	    else {
		fprintf(
		    stderr,
		    "Conclusion: binding of method '%s' in '%s' is '%s'\n",
		    snm,
		    where_class->name,
		    res->name->name
		);
	    }
	    rfre_tmstring( snm );
	}
	return res;
    }
    if( candidates->sz > 1 ){
	tmstring snm1 = typename_Signature( s );
	origin_error( org, "Method reference '%s' is ambiguous", snm1 );
	rfre_tmstring( snm1 );
	for( unsigned int ix=0; ix<candidates->sz; ix++ ){
	    tmstring snm = typename_Signature( candidates->arr[ix]->realname );

	    error( "Possible method: '%s'", snm );
	    rfre_tmstring( snm );
	}
	error( "You can resolve this by qualifying the name." );
    }
    Entry e = lookup_Entry( symtab, candidates->arr[0]->mangledname );
    assert( is_MethodEntry( e ) );
    res = to_MethodEntry( e );
    modflags method_flags = candidates->arr[0]->realname->flags;
    *is_static = ((method_flags & ACC_STATIC) != 0);
    if( has_any_flag( method_flags, ACC_NATIVE|ACC_FINAL|ACC_STATIC ) ){
	*use_static_call = true;
	if( showmethodbinding ){
	    tmstring snm = typename_Signature( s );
	    fprintf(
		stderr,
		"Invocation of %s(=%s) can use a static call because it is",
		snm,
		res->name->name
	    );
	    if( method_flags & ACC_NATIVE ){
		fputs( " native", stderr );
	    }
	    if( method_flags & ACC_STATIC ){
		fputs( " static", stderr );
	    }
	    if( method_flags & ACC_FINAL ){
		fputs( " final", stderr );
	    }
	    fputs( "\n", stderr );
	    rfre_tmstring( snm );
	}
    }
    else {
	*use_static_call = false;
	if( showmethodbinding ){
	    tmstring snm = typename_Signature( s );
	    fprintf(
		stderr,
		"Invocation of %s(=%s) must use a dynamic call\n",
		snm,
		res->name->name
	    );
	    rfre_tmstring( snm );
	}
    }
    rfre_MethodMapping_list( candidates );
    if( showmethodbinding ){
	tmstring snm = typename_Signature( s );

	fprintf(
	    stderr,
	    "Conclusion: binding of method '%s' in '%s' is '%s'\n",
	    snm,
	    where_class->name,
	    res->name->name
	);
	rfre_tmstring( snm );
    }
    return res;
}

static MethodEntry search_Method(
 SparProgram *prog,
 const_Entry_list symtab,
 const_TypeEntry_list typelist,
 const_type where_class,		/* From which class is this invoked. */
 const tmsymbol invocation_class,	/* In which class is the method. */
 const_Signature s,			/* Desired signature of the method. */
 bool *is_static,
 bool *use_static_call,			// static call can/must be used?
 bool *rejected,
 const_origin org
)
{
    if( where_class == typeNIL ){
	return MethodEntryNIL;
    }
    assert( where_class->tag == TAGObjectType );
    return search_Method(
	prog,
	symtab,
	typelist,
	to_const_ObjectType(where_class)->name->sym,
	invocation_class,
	s,
	is_static,
	use_static_call,
	rejected,
	org
    );
}

/* Given a symbol table 'symtab', the name of the class the method is
 * expected to be in, the actual method signature 's', and a pointer
 * to a method name 'nmp', return the name of the method that
 * best matches the given signature. The matching process is
 * described in JLS2 15.12.2.2.
 *
 * If two or more functions are equally specific, an error message is printed,
 * and an arbitrary one is returned.
 */
MethodEntry bind_Method(
 SparProgram *prog,
 const_Entry_list symtab,
 const_TypeEntry_list typelist,
 const tmsymbol where_class,		// In which class is the method.
 const tmsymbol invocation_class,	// From which class is this invoked.
 const_Signature s,			// Desired signature of the method.
 bool *is_static,
 bool *use_static_call,			// static call can/must be used?
 const_origin org
)
{
    bool rejected = false;

    MethodEntry res = search_Method(
	prog,
	symtab,
	typelist,
	where_class,
	invocation_class,
	s,
	is_static,
	use_static_call,
	&rejected,
	org
    );
    if( res == MethodEntryNIL ){
	tmstring snm = typename_Signature( s );
	if( rejected ){
	    origin_error(
		org,
		"%s only matches inaccessible methods in %s",
		snm,
		where_class->name
	    );
	}
	else {
	    origin_error( org, "no method in %s matches %s", where_class->name, snm );
	}
	rfre_tmstring( snm );
	return MethodEntryNIL;
    }
    if( showmethodbinding ){
	tmstring snm = typename_Signature( s );

	fprintf( stderr, "Binding of %s is %s\n", snm, res->name->name );
	rfre_tmstring( snm );
    }
    return res;
}

/* Given a symbol table 'symtab', the name of the class the method is
 * expected to be in, the actual method signature 's', and a pointer
 * to a method name 'nmp', return the name of the method that
 * best matches the given signature. The matching process is
 * described in JLS2 15.12.2.2.
 *
 * If where_class is null, the global functions are searched.
 *
 * If two or more functions are equally specific, an error message is printed,
 * and an arbitrary one is returned.
 */
tmsymbol bind_Constructor(
 SparProgram *prog,
 const_Entry_list symtab,		/* The symbol table */
 const_TypeEntry_list typelist,		/* The list of types */
 const tmsymbol where_class,		/* From which class is this invoked. */
 const tmsymbol invocation_class,	/* In which class is the method. */
 const_Signature s,			/* Desired signature of the method. */
 const_origin org
)
{
    tmsymbol res;
    unsigned int ix;
    bool rejected = false;
    bool ok;

    if( where_class == tmsymbolNIL ){
	internal_error( "No class specified" );
    }
    if( showmethodbinding ){
	tmstring snm = typename_Signature( s );

	fprintf(
	    stderr,
	    "Looking for binding of constructor '%s' in class '%s', invoked from class '%s'\n",
	    snm,
	    where_class->name,
	    invocation_class?invocation_class->name:"(none)"
	);
	rfre_tmstring( snm );
    }
    ok = compile_type( prog, invocation_class );
    if( !ok ){
	return tmsymbolNIL;
    }
    ok = compile_type( prog, where_class );
    if( !ok ){
	return tmsymbolNIL;
    }
    MethodMapping_list candidates = new_MethodMapping_list();
    TypeEntry cl = lookup_TypeEntry( typelist, where_class );
    if( cl->tag != TAGClassEntry ){
	return tmsymbolNIL;
    }
    MethodMapping_list constructors = to_ClassEntry( cl )->constructors;
    for( ix=0; ix<constructors->sz; ix++ ){
	MethodMapping cons = constructors->arr[ix];

	if(
	    is_applicable_method(
		symtab,
		typelist,
		s,
		cons->realname,
		where_class,
		invocation_class,
		&rejected
	    )
	){
	    candidates = update_candidate_list( symtab, typelist, candidates, cons );
	}
    }
    if( candidates->sz == 0 ){
	tmstring snm = typename_Signature( s );
	if( rejected ){
	    origin_error(
		org,
		"%s matches an inaccessible constructor in %s",
		snm,
		where_class->name
	    );
	}
	else {
	    origin_error(
		org,
		"no constructor in %s matches %s",
		where_class->name,
		snm
	    );
	}
	rfre_tmstring( snm );
	return tmsymbolNIL;
    }
    if( candidates->sz > 1 ){
	tmstring snm1 = typename_Signature( s );
	origin_error( org, "Constructor reference '%s' is ambiguous", snm1 );
	rfre_tmstring( snm1 );
	for( ix=0; ix<candidates->sz; ix++ ){
	    tmstring snm = typename_Signature( candidates->arr[ix]->realname );

	    error( "Possible constructor: '%s'", snm );
	    rfre_tmstring( snm );
	}
    }
    res = candidates->arr[0]->mangledname;
    rfre_MethodMapping_list( candidates );
    if( showmethodbinding ){
	tmstring snm = typename_Signature( s );

	fprintf( stderr, "Binding of constructor %s is %s\n", snm, res->name );
	rfre_tmstring( snm );
    }
    return res;
}

/* Given a signature 's', return true if the symbol has a translation, and
 * set *pos to the index in the list, or return false if the symbol
 * has no translation, and do not touch *pos.
 */
bool search_MethodMapping(
    const_MethodMapping_list trans,
    const_Signature s,
    unsigned int *pos
)
{
    for( unsigned int ix=0; ix<trans->sz; ix++ ){
	if( isequal_Signature( trans->arr[ix]->realname, s ) ){
	    *pos = ix;
	    return true;
	}
    }
    return false;
}

// Given a type entry 'e' and a signature 's',
// return the mangled name of the method in the given type, or return
// tmsymbolNIL if it cannot be found.
static tmsymbol lookup_mangledname_type( const_TypeEntry e, const_Signature s )
{
    if( !is_ConcreteTypeEntry( e ) ){
	return tmsymbolNIL;
    }
    const_MethodMapping_list mappings = to_const_ConcreteTypeEntry(e)->methods;
    unsigned int pos;
    if( !search_MethodMapping( mappings, s, &pos ) ){
	return tmsymbolNIL;
    }
    return mappings->arr[pos]->mangledname;
}

// Given a type list, a type name 't', and a signature 's',
// return the mangled name of the method in the given type, or return
// tmsymbolNIL if it cannot be found.
tmsymbol lookup_mangledname_type(
 const_TypeEntry_list typelist,
 const tmsymbol t,
 const_Signature s
)
{
    TypeEntry e = lookup_TypeEntry( typelist, t );
    if( e == TypeEntryNIL ){
	return tmsymbolNIL;
    }
    return lookup_mangledname_type( e, s );
}

// Given a symbol table entry 'e', return the init expression, or
// expressionNIL if it doesn't have one.
const_expression get_init_expression( const_Entry e )
{
    optexpression res = optexpressionNIL;
    switch( e->tag ){
	case TAGGlobalVariableEntry:
	    res = to_const_GlobalVariableEntry( e )->init;
	    break;

	case TAGLocalVariableEntry:
	    res = to_const_LocalVariableEntry( e )->init;
	    break;

	case TAGFieldEntry:
	    res = to_const_FieldEntry( e )->init;
	    break;

	case TAGFunctionEntry:
	case TAGFormalParameterEntry:
	case TAGCardinalityVariableEntry:
	case TAGGCLinkEntry:
	case TAGGCTopLinkEntry:
	case TAGExternalFunctionEntry:
	case TAGAbstractFunctionEntry:
	case TAGForwardFunctionEntry:
	case TAGForwardFieldEntry:
	    break;

    }
    if( res != optexpressionNIL && res->tag == TAGOptExpr ){
	return to_OptExpr( res )->x;
    }
    return expressionNIL;
}

// Given a class 'e', the list of known types 'types' and the list of
// known variables and functions 'symtab', return the list of all methods
// of this class and its superclasses.
MethodInfo_list construct_MethodInfo_list(
 const_ClassEntry e,
 const_TypeEntry_list types,
 const_Entry_list symtab
)
{
    MethodInfo_list res;

    if( trace_vtable ){
	fprintf( stderr, "Constructing method info list for class '%s'\n", e->name->sym->name );
    }
    if( e->super == typeNIL ){
	res = new_MethodInfo_list();
    }
    else {
	// First construct the MethodInfo_list of the superclass.
        const_TypeEntry se = search_TypeEntry( types, e->super );
	if( se == TypeEntryNIL ){
	    internal_error( "Unknown superclass" );
	}
	if( se->tag != TAGClassEntry ){
	    // This is wrong, but let somebody else complain.
	    //internal_error( "superclass is not a class" );
	    res = new_MethodInfo_list();
	}
	else {
	    res = construct_MethodInfo_list( to_const_ClassEntry(se), types, symtab );
	}
    }
    // The superclass methods go up to here.
    const unsigned int superix = res->sz;
    MethodMapping_list methods = e->methods;
    for( unsigned int ix=0; ix<methods->sz; ix++ ){
	MethodMapping me = methods->arr[ix];
	const_MethodEntry method = lookup_MethodEntry( symtab, me->mangledname );

	Signature s = rdup_Signature( me->realname );

	// Remove superclass methods with the same signature
	// as this entry, since they are overridden.
	unsigned int backix = 0;
	while( backix<superix ){
	    MethodInfo se = res->arr[backix];

	    if( isequal_Signature( se->s, s ) ){
		// We override this method.
		if( trace_vtable ){
		    tmstring tstr = typename_Signature( s );
		    fprintf(
			stderr,
			"method %s in %s overrides the one in %s\n",
			tstr,
			e->name->sym->name,
			se->owner->name
		    );
		    rfre_tmstring( tstr );
		}
		// TODO: verify that the throws clause of the overridden
		// method is compatible.
		res = delete_MethodInfo_list( res, backix );
	    }
	    else {
		backix++;
	    }
	}
	res = append_MethodInfo_list(
	    res,
	    new_MethodInfo(
		s,
		e->name->sym,
		rdup_type_list( method->throws )
	    )
	);
	if( trace_vtable ){
	    tmstring tstr = typename_Signature( me->realname );
	    fprintf(
		stderr,
		"%s.%s added to method info list\n",
		e->name->sym->name,
		tstr
	    );
	    rfre_tmstring( tstr );
	}
    }
    if( trace_vtable ){
	fprintf( stderr, "method info list for '%s' completed\n", e->name->sym->name );
    }
    return res;
}

// Given a class 'e', the list of known types 'types' and the list of
// known variables and functions 'symtab', return the virtual function
// table of this class.
VtableEntry_list construct_Class_vtable(
 const_ClassEntry e,
 const_TypeEntry_list types,
 const_Entry_list symtab
)
{
    VtableEntry_list res;

    if( trace_vtable ){
	fprintf( stderr, "Constructing vtable for class '%s'\n", e->name->sym->name );
    }
    if( e->super == typeNIL ){
	res = new_VtableEntry_list();
    }
    else {
	// First construct the Vtable of the superclass.
        const_TypeEntry se = search_TypeEntry( types, e->super );
	if( se == TypeEntryNIL ){
	    internal_error( "Unknown superclass" );
	}
	if( se->tag != TAGClassEntry ){
	    internal_error( "superclass is not a class" );
	}
	res = construct_Class_vtable( to_const_ClassEntry(se), types, symtab );
    }
    // The superclass methods go up to here.
    const unsigned int superix = res->sz;
    MethodMapping_list methods = e->methods;
    for( unsigned int ix=0; ix<methods->sz; ix++ ){
	MethodMapping me = methods->arr[ix];
	const_MethodEntry method = lookup_MethodEntry( symtab, me->mangledname );

	if( method->isvirtuallyused ){
	    Signature s = rdup_Signature( me->realname );

	    // Only point to an actual method if it isn't abstract.
	    tmsymbol dest = me->mangledname;
	    if( has_any_flag( method->flags, ACC_ABSTRACT ) ){
		dest = tmsymbolNIL;
	    }
	    // Overwrite/override superclass methods with the same signature
	    // as this entry so that they point to the method of this class.
	    for( unsigned int backix=0; backix<superix; backix++ ){
		VtableEntry se = res->arr[backix];

		if( isequal_Signature( se->t, s ) ){
		    // Yep, we override this method.
		    se->realname = dest;
		    if( trace_vtable ){
			tmstring tstr = typename_Signature( s );
			fprintf(
			    stderr,
			    "%s now points to %s (override)\n",
			    tstr,
			    dest?dest->name:"(null)"
			);
			rfre_tmstring( tstr );
		    }
		}
	    }
	    res = append_VtableEntry_list(
		res,
		new_VtableEntry(
		    me->mangledname,
		    append_type_list(
			new_type_list(),
			new_ObjectType( rdup_origsymbol( e->name ) )
		    ),
		    s,
		    rdup_type( to_const_MethodEntry( method )->t ),
		    dest,
		    to_const_MethodEntry( method )->isoverridden
		)
	    );
	    if( trace_vtable ){
		tmstring tstr = typename_Signature( me->realname );
		fprintf(
		    stderr,
		    "%s.%s points to %s\n",
		    e->name->sym->name,
		    tstr,
		    dest?dest->name:"(null)"
		);
		rfre_tmstring( tstr );
	    }
	}
	else {
	    if( trace_vtable ){
		tmstring tstr = typename_Signature( me->realname );
		fprintf(
		    stderr,
		    "%s.%s does not need an entry because it is not used virtually\n",
		    e->name->sym->name,
		    tstr
		);
		rfre_tmstring( tstr );
	    }
	}
    }
    if( trace_vtable ){
	fprintf( stderr, "vtable for '%s' completed\n", e->name->sym->name );
    }
    return res;
}

// Given an existing list of vtable entries 'l', an interface 'e', the list
// of known types 'types' and the list of known variables and functions
// 'symtab', append the virtual functions of this interface to 'l'.
//
// Note that we do not recursively construct the interface tables since
// the same superinterface may be referenced several times. Making a
// unique list of interfaces is already done in collect_interfaces(),
// and in there it is much easier than doing it in here.
static VtableEntry_list construct_local_Interface_vtable(
 VtableEntry_list l,
 const_ConcreteTypeEntry e,
 const_Entry_list symtab
)
{
    if( trace_vtable ){
	fprintf(
	    stderr,
	    "Constructing local vtable for interface '%s'\n",
	    e->name->sym->name
	);
    }
    // Methods in the superinterfaces go up to here.
    unsigned int superix = l->sz;
    MethodMapping_list methods = e->methods;
    for( unsigned int ix=0; ix<methods->sz; ix++ ){
	MethodMapping me = methods->arr[ix];
	MethodEntry method = lookup_MethodEntry( symtab, me->mangledname );

	if( method->isvirtuallyused ){
	    Signature s = rdup_Signature( me->realname );

	    // Only point to an actual method if it isn't abstract.
	    tmsymbol dest = me->mangledname;
	    if( has_any_flag( method->flags, ACC_ABSTRACT ) ){
		dest = tmsymbolNIL;
	    }

	    // Overwrite/override superclass methods with the same signature
	    // as this entry so that they point to the method of this class.
	    for( unsigned int backix=0; backix<superix; backix++ ){
		VtableEntry se = l->arr[backix];
		assert( se->virtualname != me->mangledname );
		if( isequal_Signature( se->t, s ) ){
		    // Yep, we override this method.
		    se->realname = dest;
		    if( trace_vtable ){
			tmstring tstr = typename_Signature( s );
			fprintf(
			    stderr,
			    "%s now points to %s (override)\n",
			    tstr,
			    dest?dest->name:"(null)"
			);
			rfre_tmstring( tstr );
		    }
		}
	    }
	    l = append_VtableEntry_list(
		l,
		new_VtableEntry(
		    me->mangledname,
		    append_type_list(
			new_type_list(),
			new_ObjectType( rdup_origsymbol( e->name ) )
		    ),
		    s,
		    rdup_type( to_MethodEntry( method )->t ),
		    me->mangledname,
		    to_MethodEntry( method )->isoverridden
		)
	    );
	    if( trace_vtable ){
		tmstring tstr = typename_Signature( me->realname );
		fprintf(
		    stderr,
		    "%s.%s points to %s\n",
		    e->name->sym->name,
		    tstr,
		    dest?dest->name:"(null)"
		);
		rfre_tmstring( tstr );
	    }
	}
    }
    if( trace_vtable ){
	fprintf( stderr, "vtable for '%s' completed\n", e->name->sym->name );
    }
    return l;
}

static VtableEntry_list construct_local_Interface_vtable(
 const_TypeEntry_list typelist,
 VtableEntry_list l,
 const tmsymbol cl,
 const_Entry_list symtab
)
{
    TypeEntry e = lookup_TypeEntry( typelist, cl );
    if( !is_ConcreteTypeEntry( e ) ){
	internal_error( "Can only construct an interface vtable of a concrete type" );
    }
    return construct_local_Interface_vtable( l, to_ConcreteTypeEntry( e ), symtab );
}

// Given the list of types, the symbol table and a class or interface name
// 'cl', return a Vtable listing the interface methods.
VtableEntry_list construct_Interface_vtable(
    const_TypeEntry_list typelist,
    const_Entry_list symtab,
    tmsymbol cl
)
{
    TypeEntry e = lookup_TypeEntry( typelist, cl );
    if( !is_ConcreteTypeEntry( e ) ){
	internal_error( "Can only construct an interface vtable of a concrete type" );
    }
    ConcreteTypeEntry ce = to_ConcreteTypeEntry( e );
    tmsymbol_list all_interfaces = collect_interfaces(
	typelist,
	origsymbol_listNIL,
	new_tmsymbol_list(),
	ce
    );
    switch( e->tag ){
	case TAGInterfaceEntry:
	    all_interfaces = append_tmsymbol_list( all_interfaces, cl );
	    break;

	case TAGClassEntry:
	case TAGPackageEntry:
	case TAGForwardObjectEntry:
	    break;
    }
    VtableEntry_list vtable = new_VtableEntry_list();
    for( unsigned int ix=0; ix<all_interfaces->sz; ix++ ){
	vtable = construct_local_Interface_vtable(
	    typelist,
	    vtable,
	    all_interfaces->arr[ix],
	    symtab
	);
    }
    rfre_tmsymbol_list( all_interfaces );
    return vtable;
}

// Forward declaration.
static tmsymbol search_method_implementation(
 const_TypeEntry_list typelist,
 const_Signature s,
 const_type t
);

// Given a signature 's' and a type name 'tnm', return the mangled name
// of the method that implements the method with signature 's' in type
// 'tnm', or return MethodEntryNIL if there is no such method.
static tmsymbol search_method_implementation(
 const_TypeEntry_list typelist,
 const_Signature s,
 const_TypeEntry te
)
{
    switch( te->tag ){
	case TAGClassEntry:
	{
	    const_ClassEntry ce = to_const_ClassEntry( te );
	    const_MethodMapping_list methods = ce->methods;
	    unsigned int pos;

	    if( search_MethodMapping( methods, s, &pos ) ){
		// We've found a method that matches
		return methods->arr[pos]->mangledname;
	    }
	    // The method is not implemented in this class, try
	    // the superclass
	    if( ce->super != typeNIL ){
		return search_method_implementation( typelist, s, ce->super );
	    }
	    break;
	}

	case TAGInterfaceEntry:
	case TAGForwardObjectEntry:
	case TAGPackageEntry:
	    break;


    }
    return tmsymbolNIL;
}

// Given a signature 's' and a type name 'tnm', return the MethodEntry
// of the method that implements the method with signature 's' in type
// 'tnm', or return MethodEntryNIL if there is no such method.
static tmsymbol search_method_implementation(
 const_TypeEntry_list typelist,
 const_Signature s,
 const_origsymbol tnm
)
{
    const_TypeEntry te = lookup_TypeEntry( typelist, tnm );
    if( te == TypeEntryNIL ){
	return tmsymbolNIL;
    }
    return search_method_implementation( typelist, s, te );
}

static tmsymbol search_method_implementation(
 const_TypeEntry_list typelist,
 const_Signature s,
 const_type t
)
{
    assert( t->tag == TAGObjectType );
    return search_method_implementation( typelist, s, to_const_ObjectType(t)->name );
}

// Given a method entry, search for the method, if any, that is overridden
// by this method.
tmsymbol search_overridden( const_TypeEntry_list typelist, const_MethodEntry me )
{
    if( me == MethodEntryNIL ){
	return tmsymbolNIL;
    }
    const_TypeEntry te = lookup_TypeEntry( typelist, me->owner );
    if( te->tag != TAGClassEntry ){
	return tmsymbolNIL;
    }
    const_type super = to_const_ClassEntry(te)->super;
    if( super == typeNIL ){
	return tmsymbolNIL;
    }
    return search_method_implementation( typelist, me->realname, super );
}

/* Given a field translation 'from' to 'to', add it to the translation list. */
static FieldMapping_list add_Field_translation(
 FieldMapping_list mappings,
 tmsymbol ctx,
 const FieldMapping mapping
)
{
    mappings = append_FieldMapping_list( mappings, mapping );
    if( showmangling ){
	fprintf(
	    stderr,
	    "Add to context '%s': field mapping %s->%s\n",
	    ctx->name,
	    mapping->realname->name->name,
	    mapping->mangledname->name
	);
    }
    return mappings;
}

/* Given a symbol of a (non-static) field definition, create and enter
 * its translation in the list.
 */
FieldMapping_list build_Field_translation(
 FieldMapping_list mappings,
 tmsymbol owner,
 const_Field s
)
{
    tmsymbol nw = qualify_tmsymbol( owner, s->name );

    mappings = add_Field_translation(
	mappings,
	owner,
	new_FieldMapping( rdup_Field( s ), gen_fqname( nw ), owner )
    );
    return mappings;
}

/* Given a list of symbols that are for (non-static) field definitions, enter
 * them in the list of translations.
 */
FieldMapping_list build_Field_translations(
 FieldMapping_list mappings,
 tmsymbol owner,
 const_Field_list fl
)
{
    for( unsigned int ix=0; ix<fl->sz; ix++ ){
	mappings = build_Field_translation( mappings, owner, fl->arr[ix] );
    }
    return mappings;
}

// Given a type 't', return true iff the type should be garbage-collected.
bool is_collectable_type( const_type t )
{
     if( t == typeNIL ){
	 return false;
     }
     switch( t->tag ){
	  case TAGObjectType:
	  case TAGGenericObjectType:
	  case TAGExceptionVariableType:
	      return true;

	  case TAGArrayType:
	      return true;

	  case TAGTupleType:
	      // TODO: What about elements that are collectable?
	      return false;

	  case TAGVectorType:
	      return is_collectable_type( to_const_VectorType(t)->elmtype );

	  case TAGPragmaType:
	      return is_collectable_type( to_const_PragmaType(t)->t );

	  case TAGTypeOfIf:
	      internal_error( "TypeOfIf should have been rewritten" );
	      break;

	  case TAGTypeOf:
	      internal_error( "TypeOf should have been rewritten" );
	      break;

	  case TAGPrimArrayType:
	  case TAGVoidType:
	  case TAGNullType:
	  case TAGFunctionType:
	  case TAGPrimitiveType:
	  case TAGGCRefLinkType:
	  case TAGGCTopRefLinkType:
	  case TAGTypeType:
	      break;

     }
     return false;
}

// Given a symboltable entry and a scope, return true iff it needs to be added
// to the GC links of that scope.
//
// The function takes up about 20% of the compiler execution time,
// so it is inlined.
static inline bool is_GC_link( const_Entry e, tmsymbol scope )
{
    bool res = false;

    switch( e->tag ){
	case TAGLocalVariableEntry:
	{
	    const_LocalVariableEntry le = to_const_LocalVariableEntry(e);

	    if( le->scope == scope ){
		res = le->isused && is_collectable_type( le->t );
	    }
	    break;
	}

	case TAGFormalParameterEntry:
	{
	    const_FormalParameterEntry fe = to_const_FormalParameterEntry(e);
	    if( fe->scope == scope ){
		res = fe->isused && is_collectable_type( fe->t );
	    }
	    break;
	}

	case TAGGCLinkEntry:
	case TAGGCTopLinkEntry:
	case TAGGlobalVariableEntry:
	case TAGFieldEntry:
	case TAGFunctionEntry:
	case TAGCardinalityVariableEntry:
	case TAGExternalFunctionEntry:
	case TAGAbstractFunctionEntry:
	case TAGForwardFunctionEntry:
	case TAGForwardFieldEntry:
	    break;
    }
    return res;
}

// Given a scope name, return the elements of the GC reference set in
// this scope. Do not return unused variables.
tmsymbol_list get_GC_links( const_Entry_list symtab, tmsymbol scope )
{
    tmsymbol_list res = new_tmsymbol_list();

    for( unsigned int ix=0; ix<symtab->sz; ix++ ){
	const_Entry e = symtab->arr[ix];

	if( is_GC_link( e, scope ) ){
	    res = append_tmsymbol_list( res, e->name );
	}
    }
    return res;
}

static bool is_global_GC_link( const_Entry e )
{
    bool res = false;

    switch( e->tag ){
	case TAGGlobalVariableEntry:
	{
	    const_GlobalVariableEntry ge = to_const_GlobalVariableEntry(e);

	    res = ge->isused && is_collectable_type( ge->t );
	    break;
	}

	case TAGFormalParameterEntry:
	case TAGLocalVariableEntry:
	case TAGGCLinkEntry:
	case TAGGCTopLinkEntry:
	case TAGFieldEntry:
	case TAGFunctionEntry:
	case TAGCardinalityVariableEntry:
	case TAGExternalFunctionEntry:
	case TAGAbstractFunctionEntry:
	case TAGForwardFunctionEntry:
	case TAGForwardFieldEntry:
	    break;
    }
    return res;
}

// Return the elements of the global GC reference set.
// Do not return unused variables.
tmsymbol_list get_global_GC_links( const_Entry_list symtab )
{
    tmsymbol_list res = new_tmsymbol_list();

    for( unsigned int ix=0; ix<symtab->sz; ix++ ){
	const_Entry e = symtab->arr[ix];
	if( is_global_GC_link( e ) ){
	    res = append_tmsymbol_list( res, e->name );
	}
    }
    return res;
}

// Given a symbol table entry, we know that it is refered to in a GC
// reference list, so make sure that it is initialized
// to something, if only NULL.
static void null_GC_link( Entry e )
{
    if( e->tag != TAGLocalVariableEntry ){
	// Only local variable entries need treatment.
	return;
    }
    LocalVariableEntry ve = to_LocalVariableEntry(e);
    if( ve->init->tag == TAGOptExpr ){
	// Variable is already initialized, don't do anything.
	return;
    }
    rfre_optexpression( ve->init );
    ve->init = new_OptExpr(
	new_CastExpression( rdup_type( ve->t ), new_NullExpression() )
    );
    // This variable is not final in the sense of Vnus, since we
    // initialize it first, and assign something to it later.
    ve->flags &= ~(ACC_FINAL|ACC_DEDUCED_FINAL);
}

// Given a scope name, make sure that all GC references are initialized
// to something, if only NULL.
void null_GC_links( const_Entry_list symtab, tmsymbol scope )
{
    for( unsigned int ix=0; ix<symtab->sz; ix++ ){
	Entry e = symtab->arr[ix];
	if( is_GC_link( e, scope ) ){
	    null_GC_link( e );
	}
    }
}

