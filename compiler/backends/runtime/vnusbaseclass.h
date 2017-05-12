// File: vnusbaseclass.h
//
// The class used by every Vnus program as the base class of all
// records.

#ifndef INC_VNUSBASECLASS_DONE
#define INC_VNUSBASECLASS_DONE

#include "vnustypes.h"
#include "garbagecollect.h"
#include "outofmemory.h"
#include "vnusalloc.h"

#include <string.h>

class VnusBase {
public:
    typedef void (*markfntype)( VnusBase * );

    VnusBase() {}
    virtual inline ~VnusBase() {}
    inline void operator delete( void *p, size_t /* sz */ ) { vnus_free( p ); }
    VnusBase &operator=( const VnusBase & ) { return *this; }

    inline void setMark( VnusBoolean val ){ vnus_alloc_setmark( this, val ); }
    inline VnusBoolean getMark() const { return vnus_alloc_getmark( this ); }

    inline void *operator new( size_t sz ) {
	// TODO: see if we can sometimes set the 'nulled' flag to false.
	return vnus_alloc( sz, true );
    }

    inline void *operator new( size_t sz, size_t extrabytes, bool filled ) {
	return vnus_alloc( sz+extrabytes, filled );
    }
    virtual markfntype getShapeMarkfn() const { return (markfntype) NULL; }
    virtual bool isShape() const { return false; }

};
#else
class VnusBase;
#endif
