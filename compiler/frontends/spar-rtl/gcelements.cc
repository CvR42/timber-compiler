// File: gcelements.cc

#include "spar-rtl.h"
#include "fakeObject.h"

void Java_spar_compiler_GC_invokeMarkingMethod( VnusBase *obj )
{
    if( obj->isShape() ){
	VnusBase::markfntype ptr = obj->getShapeMarkfn();
	if( ptr != NULL ){
	    // There is a shape marker function, invoke it.
	    (*ptr)( obj );
	}
	return;
    }
    ObjectClass *o = (ObjectClass *) obj;
    if( o->vtable->introspection->mark != NULL ){
	(*o->vtable->introspection->mark)( o );
    }
}

void Java_spar_compiler_GC_invokeFinalize( VnusBase *obj )
{
    if( !obj->isShape() ){
	ObjectClass *o = (ObjectClass *) obj;

	if( o->vtable->introspection->finalizer != NULL ){
	    (*o->vtable->introspection->finalizer)( o );
	}
    }
}
