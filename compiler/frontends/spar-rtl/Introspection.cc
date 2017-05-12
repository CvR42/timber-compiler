// File: Introspection.cc

#include <vnustypes.h>
#include <vnusbaseclass.h>
#include <vnusstd.h>
#include "spar-rtl.h"
#include "fakeObject.h"
#include "getclassstring.h"

#define ACC_PUBLIC              0x00000001
#define ACC_PRIVATE             0x00000002
#define ACC_PROTECTED           0x00000004
#define ACC_STATIC              0x00000008
#define ACC_FINAL               0x00000010
#define ACC_SYNCHRONIZED        0x00000020
#define ACC_VOLATILE            0x00000040
#define ACC_TRANSIENT           0x00000080
#define ACC_NATIVE              0x00000100
#define ACC_INTERFACE           0x00000200
#define ACC_ABSTRACT            0x00000400

#define ACC_PRIMITIVE		0x10000000

extern IntrospectionRecord *__spar_rte_typelist[];
extern VnusInt __spar_rte_typelist_length;

// Given an object, return the class id associated with it.
VnusInt Java_spar_compiler_Introspection_getClassId( void *obj )
{
    ObjectClass *oo = (ObjectClass *) obj;
    VTable *vt = oo->vtable;
    IntrospectionRecord *intro = vt->introspection;

    return intro->classno;
}

static IntrospectionRecord *search_intro_record( VnusInt id )
{
    for( VnusInt ix=0; ix<__spar_rte_typelist_length; ix++ ){
	IntrospectionRecord *r = __spar_rte_typelist[ix];
	if( r->classno == id ){
	    return r;
	}
    }
    return NULL;
}

static IntrospectionRecord *search_intro_record_name( const char *nm )
{
    for( VnusInt ix=0; ix<__spar_rte_typelist_length; ix++ ){
	IntrospectionRecord *r = __spar_rte_typelist[ix];
	if( strcmp( nm, r->classnm ) == 0 ){
	    return r;
	}
    }
    return NULL;
}

VnusString Java_spar_compiler_Introspection_getClassName( VnusInt id )
{
    IntrospectionRecord *r = search_intro_record( id );
    if( r != NULL ){
	return r->classnm;
    }
    return "??";
}

VnusInt Java_spar_compiler_Introspection_getModifiers( VnusInt id )
{
    IntrospectionRecord *r = search_intro_record( id );
    if( r != NULL ){
	return r->flags;
    }
    return -1;
}

VnusBoolean Java_spar_compiler_Introspection_isInterface( VnusInt id )
{
    IntrospectionRecord *r = search_intro_record( id );
    if( r != NULL ){
	return (r->flags & ACC_INTERFACE) != 0;
    }
    return false;
}

VnusBoolean Java_spar_compiler_Introspection_isPrimitive( VnusInt id )
{
    IntrospectionRecord *r = search_intro_record( id );
    if( r != NULL ){
	return (r->flags & ACC_PRIMITIVE) != 0;
    }
    return false;
}

VnusInt Java_spar_compiler_Introspection_ClassIdFromString1( VnusChar *nm, VnusInt len )
{
    char *nm_string = get_class_string( nm, len );
    const IntrospectionRecord *r = search_intro_record_name( nm_string );
    free( nm_string );
    if( r == NULL ){
	return -1;
    }
    return r->classno;
}
