// File: vnusnotnullassert.h

#ifndef INC_VNUSNOTNULLASSERT_H
#define INC_VNUSNOTNULLASSERT_H

extern void VnusEventNullPointer();

template <class T> inline T *VnusNotNullAssert( T *p )
{
    if( VNUS_UNLIKELY( p == NULL ) ){
	VnusEventNullPointer();
    }
    return p;
}

#endif
