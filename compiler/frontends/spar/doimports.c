/* File: doimports.c
 *
 * Handle imports.
 */

#include <string.h>
#include <tmc.h>

#include "defs.h"
#include "tmadmin.h"
#include "doimports.h"
#include "symbol_table.h"
#include "service.h"
#include "error.h"
#include "global.h"
#include "parser.h"
#include "fwdregtype.h"
#include "regtype.h"
#include "lower.h"
#include "config.h"

static int import_indent = 1;

/* Given a list of type bindings and a short type name, search the
 * list of type bindings for this type. Return the long name of the
 * type if it is in the list, else return tmsymbolNIL.
 */
tmsymbol search_TypeBinding( const_TypeBinding_list tl, tmsymbol s )
{
    // Search from back to front to make sure that the last declaration
    // of a type gets the binding. This is only interesting for local
    // types, but there it *is* significant.
    unsigned int ix = tl->sz;

    while( ix>0 ){
	ix--;

	if( tl->arr[ix]->shortname == s ){
	    return tl->arr[ix]->longname;
	}
    }
    return tmsymbolNIL;
}

/* Given a type name, return a string representing the path to the
 * file of that type, relative to the class root.
 *
 * TODO: name mangling for characters not supported by the
 * operating system.
 */
static tmstring class_path( tmsymbol t )
{
    tmstring s = new_tmstring( t->name );
    char *p = (char *) s;

    while( *p != '\0' ){
	if( *p == '.' ){
	    *p = PATHSEP;
	}
	p++;
    }
    return s;
}

/* Given a file path and a suffix, return the suffixed filename if it
 * exisits as a file, or 0 if it doesn't.
 */
static tmstring probe_file( const char *path, const char *suffix )
{
    tmstring buf = create_tmstring( strlen( path )+2+strlen( suffix ) );

    sprintf( buf, "%s.%s", path, suffix );
    if( file_exists( buf ) ){
	return buf;
    }
    rfre_tmstring( buf );
    return tmstringNIL;
}

// Given a filename, return true iff the file has a suffix that indicates
// it is a java file (as opposed to a spar file).
bool has_java_suffix( const char *fnm )
{
    const char *xp = strrchr( fnm, '.' );
    if( xp == NULL ){
	return false;
    }
    return (strcmp( xp+1, "java" ) == 0) || (strcmp( xp+1, "jav" ) == 0);
}

// Given a filename, return true iff the file has a suffix that indicates
// it is a spar file (as opposed to a java file).
static bool has_spar_suffix( const char *fnm )
{
    const char *xp = strrchr( fnm, '.' );
    if( xp == NULL ){
	return false;
    }
    return strcmp( xp+1, "spar" ) == 0;
}

/* Given a file path, try to tack on one of the suffixes "spar"
 * and "java", and return the suffixed filename if the file exist.
 * Return 0 if neither of the files exist.
 */
static tmstring probe_sparsource( const char *path )
{
    tmstring fnm = probe_file( path, "spar" );

    if( fnm != tmstringNIL ){
	return fnm;
    }
    fnm = probe_file( path, "jav" );

    if( fnm != tmstringNIL ){
	return fnm;
    }
    return probe_file( path, "java" );
}

/* Given a type name, search the class path for a file that defines
 * this type. Return the name of the first file in the class path
 * that defines this file, or tmstringNIL if no such file can be found.
 */
static tmstring search_classpath( tmsymbol t )
{
    if( t == tmsymbolNIL ){
	return tmstringNIL;
    }
    tmstring cl_path = class_path( t );

    if( trace_imports ){
	fprintf( stderr, "Looking for the source file for type '%s'\n", t->name );
    }
    for( unsigned int ix=0; ix<sparpath->sz; ix++ ){
	tmstring path = fullpath_file( sparpath->arr[ix], cl_path );
	if( trace_imports ){
	    fprintf( stderr, "Probing path '%s'\n", path );
	}
	tmstring fnm = probe_sparsource( path );

	rfre_tmstring( path );
	if( fnm != tmstringNIL ){
	    if( trace_imports ){
		fprintf( stderr, "There is a file '%s'\n", fnm );
	    }
	    rfre_tmstring( cl_path );
	    return fnm;
	}
    }
    if( !is_qualified_tmsymbol( t ) ){
	// This is an unqualified type, so see if we can find it in the
	// current directory.
	// TODO: perhaps we should do this first? If so, just move it
	// in front of the loop over sparpath.
	tmstring path = fullpath_file( ".", cl_path );
	if( trace_imports ){
	    fprintf( stderr, "Probing path '%s'\n", path );
	}
	tmstring fnm = probe_sparsource( path );

	rfre_tmstring( path );
	if( fnm != tmstringNIL ){
	    if( trace_imports ){
		fprintf( stderr, "There is a file '%s'\n", fnm );
	    }
	    rfre_tmstring( cl_path );
	    return fnm;
	}
    }
    if( trace_imports ){
	fprintf( stderr, "Cannot find source file for '%s'\n", t->name );
    }
    rfre_tmstring( cl_path );
    return tmstringNIL;
}

// Return 'true' if the class 'java.lang.Exception' can be found. If not,
// something is is very rotten.
bool java_lang_visible()
{
    tmstring s = search_classpath( add_tmsymbol( SANITYCHECK_CLASS ) );

    if( s == tmstringNIL ){
	return false;
    }
    rfre_tmstring( s );
    return true;
}

/* Given a pointer to a SparProgram context and a file name 'fnm' that is
 * known not yet to be loaded, load that type, and add it to the
 * SparProgram administration.
 */
static bool compile_unit( SparProgram *prog, const_tmstring fnm, const tmsymbol t, bool strict_java )
{
    FILE *f;

    if( trace_files ){
	for( int i=0; i<import_indent; i++ ){
	    fputc( ' ', stderr );
	}
	fprintf( stderr, "Compiling %s\n", fnm );
	(void) fflush( stderr );
    }
    f = fopen( fnm, "r" );
    if( f == NULL ){
	internal_error( "Cannot open approved source file '%s'", fnm );
    }
    SparProgramUnit unit = parse( f, fnm, strict_java );
    fclose( f );
    if( unit == SparProgramUnitNIL ){
	return false;
    }
    import_indent++;
    unit = lower_SparProgramUnit( unit );
    fwdregtype_SparProgramUnit( prog, unit );
    regtype_SparProgramUnit( prog, unit, fnm );
    import_indent--;
    (*prog)->units = append_SparProgramUnit_list( (*prog)->units, unit );

    // Now make sure that the type has the correct 'package' declaration.
    if( is_qualified_tmsymbol( t ) ){
	tmsymbol first;
	tmsymbol last;
	break_qualified_tmsymbol( t, &first, &last );

	if(
	    unit->packagename == origsymbolNIL ||
	    unit->packagename->sym->name[0] == '\0'
	){
	    error(
		"File `%s' should declare package `%s'",
		fnm,
		first->name
	    );
	}
	else if( first != unit->packagename->sym ){
	    origsymbol_error(
		unit->packagename, 
		"unexpected package declaration. It should be '%s'",
		first->name
	    );
	}
    }
    else {
	// When we refer to this class in an unqualified manner, it
	// may still have a package declaration. We don't complain,
	// since this is an informal way to address a type anyway.
    }
    return true;
}

/* Given a pointer to a SparProgram context and a type 't' that is
 * known not yet to be loaded, load that type, and add it to the
 * SparProgram administration.
 */
static bool load_type( SparProgram *prog, const_origsymbol t )
{
    tmstring fnm = search_classpath( t->sym );
    bool res;

    if( fnm == tmstringNIL ){
	origsymbol_error( t, "Cannot find a definition for this type" );
	return false;
    }
    res = compile_unit( prog, fnm, t->sym, has_java_suffix( fnm ) );
    rfre_tmstring( fnm );
    return res;
}

/* Given a pointer to a SparProgram context, a pointer to a list
 * of type bindings, and a type name, read and parse the program
 * unit of the type, and register the type in the type bindings list.
 */
static bool register_single_import(
 SparProgram *prog,
 TypeBinding_list *bindings,
 origsymbol t
)
{
    TypeEntry e = search_TypeEntry( (*prog)->typelist, t->sym );
    bool ok = true;

    if( e == TypeEntryNIL ){
	ok = load_type( prog, t );
	if( ok ){
	    e = search_TypeEntry( (*prog)->typelist, t->sym );
	    if( e == 0 ){
		origsymbol_error(
		    t,
		    "Compiling the program unit of this type doesn't define it"
		);
		ok = false;
	    }
	}
    }
    tmsymbol shorty = shortname( t->sym );
    *bindings = register_TypeBinding(
	*bindings,
	t->org->file,
	new_TypeBinding( shorty, t->sym )
    );
    return ok;
}

bool register_single_import_list(
 SparProgram *prog,
 TypeBinding_list *bindings,
 origsymbol_list tl
)
{
    for( unsigned int ix=0; ix<tl->sz; ix++ ){
	if( !register_single_import( prog, bindings, tl->arr[ix] ) ){
	    return false;
	}
    }
    return true;
}

/* Given sufficient context to register the change, a short type name 's',
 * a list of on-demand packages, and a pointer to the full name, search for
 * a binding of the short name to a full name.
 */
static bool bind_demand_type(
 TypeEntry_list typelist,
 const_origsymbol_list ondemands,
 tmsymbol s,
 const_origin org,
 tmsymbol *fullname
)
{
    tmsymbol candidate_fullname = tmsymbolNIL;
    tmstring candidate_filename = tmstringNIL;

    if( trace_imports ){
	fprintf( stderr, "Searching on-demand packages for type '%s'\n", s->name );
    }
    /* Search the list of demand imports. Since it is an error
     * to have the same type defined in several on-demand candidates,
     * maintain sufficient administration to complain about that.
     */
    if( ondemands != origsymbol_listNIL ){
	for( unsigned int ix=0; ix<ondemands->sz; ix++ ){
	    origsymbol ondemand = ondemands->arr[ix];
	    tmsymbol new_fullname = qualify_tmsymbol( ondemand->sym, s );
	    TypeEntry e = search_TypeEntry( typelist, new_fullname );

	    if( e == TypeEntryNIL ){
		tmstring path = search_classpath( new_fullname );

		if( path != tmstringNIL ){
		    /* We have a candidate */
		    if( candidate_fullname != tmsymbolNIL ){
			/* But that's the second one! */
			origin_error(
			    org,
			    "Abiguous type: %s and %s", 
			    candidate_fullname->name,
			    new_fullname->name
			);
			rfre_tmstring( path );
		    }
		    else {
			candidate_fullname = new_fullname;
			candidate_filename = path;
		    }
		}
	    }
	    else {
		/* We have a candidate that's already in the list of types. */
		if( candidate_fullname != tmsymbolNIL ){
		    /* But that's the second one! */
		    origin_error(
			org,
			"Abiguous type: %s and %s", 
			candidate_fullname->name,
			new_fullname->name
		    );
		}
		else {
		    candidate_fullname = new_fullname;
		    candidate_filename = tmstringNIL;
		}
	    }
	}
    }
    if( trace_imports ){
	if( candidate_fullname == tmsymbolNIL ){
	    fprintf( stderr, "Cannot import '%s'\n", s->name );
	}
	else {
	    if( candidate_filename == tmstringNIL ){
		fprintf( stderr, "'%s' is already imported\n", s->name );
	    }
	    else {
		fprintf(
		    stderr,
		    "Found source file for '%s': '%s'\n",
		    s->name,
		    candidate_filename
		);
	    }
	}
    }
    rfre_tmstring( candidate_filename );
    if( candidate_fullname != tmsymbolNIL ){
	*fullname = candidate_fullname;
	return true;
    }
    return false;
}

#if 0
/* Given sufficient context to register the change, a short type name 's',
 * a list of on-demand packages, and a pointer to the full name, search for
 * a binding of the short name to a full name.
 *
 * If necessary other source files are imported to satisfy the request.
 */
static bool demand_type(
 SparProgram *prog,
 origsymbol_list ondemands,
 const origsymbol s,
 tmsymbol *fullname
)
{
    if( bind_demand_type( (*prog)->typelist, ondemands, s, fullname ) ){
	TypeEntry e = search_TypeEntry( (*prog)->typelist, *fullname );
	if( e == TypeEntryNIL ){
	    tmstring path = search_classpath( *fullname );

	    if( !compile_unit( prog, path ) ){
		rfre_tmstring( path );
		return false;
	    }
	    e = search_TypeEntry( (*prog)->typelist, *fullname );
	    if( e == TypeEntryNIL ){
		origsymbol_error(
		    s,
		    "Compiling source file '%s' doesn't define the type",
		    path
		);
	    }
	    rfre_tmstring( path );
	}
	return e != TypeEntryNIL;
    }
    return false;
}
#endif

/* Given sufficient context to register the change, a short type name 's',
 * a package name, and a pointer to the full name, see if the type is defined
 * in the package.
 */
static bool bind_thispackage_type(
 SparProgram *prog,
 tmsymbol package,
 tmsymbol s,
 tmsymbol *fullname
)
{
    tmsymbol candidate_fullname = qualify_tmsymbol( package, s );

    if( trace_imports ){
	fprintf( stderr, "Searching current package for type '%s'\n", s->name );
    }
    TypeEntry e = search_TypeEntry( (*prog)->typelist, candidate_fullname );
    if( e == TypeEntryNIL ){
	tmstring path = search_classpath( candidate_fullname );

	if( path != tmstringNIL ){
	    rfre_tmstring( path );
	    *fullname = candidate_fullname;
	    return true;
	}
	else {
	    return false;
	}
    }
    else {
	*fullname = candidate_fullname;
    }
    return true;
}

/* Given a type name 's', return the fully qualified name of the type,
 * or tmsymbolNIL if it has no translation, presumably because it is
 * not a type (or because there is an error in the program).
 *
 * If necessary other source files are imported to satisfy the request.
 *
 * If necessary register the binding of the type to the fully qualified
 * name in the type binding list 'trans'.
 */
tmsymbol canonify_type(
 SparProgram *prog,
 tmsymbol package,
 tmsymbol fromtype,
 const_origsymbol_list ondemands,
 TypeBinding_list *trans,
 const tmsymbol s,
 const_origin org
)
{
    tmsymbol fullname;
    unsigned int ix;

    if( trace_bindings ){
	fprintf( stderr, "Searching for binding of type %s\n", s->name );
    }
    tmsymbol res = search_TypeBinding( *trans, s );
    if( res != tmsymbolNIL ){
	if( trace_bindings ){
	    fprintf( stderr, "Found existing binding: '%s' ==> '%s'\n", s->name, res->name );
	}
again:
	tmsymbol res1 = search_TypeBinding( *trans, res );
	if( res1 != tmsymbolNIL && res1 != res ){
	    res = res1;
	    goto again;
	}
	return res;
    }

    // No joy with the current type bindings list. We'll have to
    // search for it.

    // First, see if it is a known type.
    if( search_TypeEntry_pos( (*prog)->typelist, s, &ix ) ){
	if( trace_bindings ){
	    fprintf( stderr, "Found binding of type '%s' - it has a type entry\n", s->name );
	}
	return s;
    }
    if( is_qualified_tmsymbol( s ) ){
	tmstring path = search_classpath( s );

	if( path != tmstringNIL ){
	    if( trace_bindings ){
		fprintf( stderr, "Found binding of type %s - it lives in file '%s'\n", s->name, path );
	    }
	    rfre_tmstring( path );
	    return s;
	}
	tmsymbol first;
	tmsymbol last;
	break_qualified_tmsymbol( s, &first, &last );
	if( trace_bindings ){
	    fprintf( stderr, "No success yet in binding '%s' - see if I can bind type '%s' first\n", s->name, first->name );
	}
	tmsymbol bound_first = canonify_type(
	    prog,
	    package,
	    fromtype,
	    ondemands,
	    trans,
	    first,
	    org
	);
	if( bound_first != tmsymbolNIL ){
	    if( trace_bindings ){
		fprintf( stderr, "No success yet in binding '%s', but I found binding '%s' ==> '%s'\n", s->name, first->name, bound_first->name );
	    }
	    tmsymbol bound_s = qualify_tmsymbol( bound_first, last );

	    if( search_TypeEntry_pos( (*prog)->typelist, bound_s, &ix ) ){
		// Register it for a faster binding next time.
		*trans = register_TypeBinding(
		    *trans,
		    org->file,
		    new_TypeBinding( s, bound_s )
		);
		if( trace_bindings ){
		    fprintf( stderr, "Found binding '%s' ==> '%s'\n", s->name, bound_s->name );
		}
		return bound_s;
	    }
	    if( trace_bindings ){
		fprintf( stderr, "I can bind '%s' to '%s', but I cannot bind '%s' to '%s'. I give up.\n", first->name, bound_first->name, s->name, bound_s->name );
	    }
	    return tmsymbolNIL;
	}
	if( trace_bindings ){
	    fprintf( stderr, "I cannot bind '%s', and I cannot bind '%s'. I give up.\n", s->name, first->name );
	}
	return tmsymbolNIL;
    }
    else if( fromtype != tmsymbolNIL ){
	// Not a qualified symbol. This *might* be a local type of a
	// superclass. Go search there.
	tmsymbol nowtype = fromtype;
	TypeEntry nowentry = lookup_TypeEntry( (*prog)->typelist, nowtype );

	while( nowentry != TypeEntryNIL ){
	    if( nowentry->tag != TAGClassEntry ){
		break;
	    }
	    const_type super = to_const_ClassEntry( nowentry )->super;

	    if( super == typeNIL ){
		break;
	    }
	    TypeEntry superentry = lookup_TypeEntry( (*prog)->typelist, super );
	    if( superentry == TypeEntryNIL ){
		break;
	    }
	    if( trace_bindings ){
		fprintf( stderr, "I cannot bind '%s' in type '%s', trying superclass %s.\n", s->name, nowtype->name, superentry->name->sym->name );
	    }
	    if( is_ConcreteTypeEntry( superentry ) ){
		TypeBinding_list bindings = to_const_ConcreteTypeEntry( superentry )->types;
		tmsymbol b = search_TypeBinding( bindings, s );
		if( b != tmsymbolNIL ){
		    // Register it for a faster binding next time.
		    *trans = register_TypeBinding(
			*trans,
			org->file,
			new_TypeBinding( s, b )
		    );
		    if( trace_bindings ){
			fprintf( stderr, "Found binding '%s' ==> '%s'\n", s->name, b->name );
		    }
		    return b;
		}

	    }
	    nowentry = superentry;
	    nowtype = superentry->name->sym;
	}
    }
    if( trace_imports || trace_bindings ){
	fprintf( stderr, "No existing binding found for type '%s'\n", s->name );
    }
    if( bind_thispackage_type( prog, package, s, &fullname ) ){
	// Register it for a faster binding next time.
	*trans = register_TypeBinding(
	    *trans,
	    org->file,
	    new_TypeBinding( s, fullname )
	);
	return fullname;
    }
    if( bind_demand_type( (*prog)->typelist, ondemands, s, org, &fullname ) ){
	// Register it for a faster binding next time.
	*trans = register_TypeBinding(
	    *trans,
	    org->file,
	    new_TypeBinding( s, fullname )
	);
	return fullname;
    }
    if( trace_bindings ){
	fprintf( stderr, "I cannot bind '%s'\n", s->name );
    }
    return tmsymbolNIL;
}

tmsymbol canonify_type(
 SparProgram *prog,
 tmsymbol package,
 tmsymbol fromtype,
 const_origsymbol_list ondemands,
 TypeBinding_list *trans,
 const_origsymbol s
)
{
    if( s == origsymbolNIL ){
	return tmsymbolNIL;
    }
    return canonify_type( prog, package, fromtype, ondemands, trans, s->sym, s->org );
}

/* Given a fully qualified type name 't', make sure that the type is
 * present in the list of compilation units of the SparProgram '*prog'.
 * If compile the source file of the type.
 */
bool compile_type( SparProgram *prog, const tmsymbol t )
{
    unsigned int pos;

    if( t == tmsymbolNIL ){
	return true;
    }
    if( search_type_SparProgramUnit_list( (*prog)->units, t, &pos ) ){
	return true;
    }
    tmstring path = search_classpath( t );
    if( path == tmstringNIL ){
	return false;
    }
    bool ok = compile_unit( prog, path, t, has_java_suffix( path ) );
    rfre_tmstring( path );
    return ok;
}

/* Given a symbol 't' representing a public type, and the name of
 * the file it is defined in 'fnm', make sure that it is findable
 * because it has the right filename.
 */
void check_findable_public( origsymbol t, const char *fnm )
{
    tmstring buf = new_tmstring( fnm );

    char *p = strrchr( buf, PATHSEP );
    if( p == NULL ){
	p = buf;
    }
    else {
	p++;
    }
    if( !has_java_suffix( p ) && !has_spar_suffix( p ) ){
	origsymbol_error( t, "A public type should be in a source file with extension 'spar' or 'java', not file '%s'", p );
	rfre_tmstring( buf );
	return;
    }
    char *xp = strrchr( p, '.' );
    if( xp == NULL ){
	origsymbol_error( t, "A public type should be in a source file with an extension, not file '%s'", p );
	rfre_tmstring( buf );
	return;
    }
    *xp = '\0';
    /* TODO: take into account mangling of difficult characters. */
    /* See also is_main_type() */
    if( strcmp( p, t->sym->name ) != 0 ){
	*xp = '.';
	origin_error(
	    t->org,
	    "A public type '%s' should be in file '%s.java' or '%s.spar', not '%s'",
	    t->sym->name,
	    t->sym->name,
	    t->sym->name,
	    p
	);
    }
    rfre_tmstring( buf );
}
