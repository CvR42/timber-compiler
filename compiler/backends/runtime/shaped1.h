// File: shaped1.h

#ifndef __SHAPED1_H__
#define __SHAPED1_H__

#include "polywrite.h"
#include "vnusstd.h"
#include "vnusservice.h"
#include "shapebase.h"
#include "arrayboundviolated.h"

#include <stdio.h>
#include <stdlib.h>

template<class T> class ShapeD1 : public ShapeBase<T>
{
private:
    inline ShapeD1( const ShapeD1<T>& theShape):
     ShapeBase<T>(theShape)
    {
    }

    // Assign one shape to another, but only if they are exactly alike.
    // This should not be used... I hope...
    ShapeD1<T>& operator= (const ShapeD1<T>& theShape)
    {
        if( this == &theShape ){
            return *this;
        }
	if( VNUS_UNLIKELY( this->length != theShape.GetSize0() ) ){
	    fprintf(stderr, "*** ERROR -- \n");
	    fprintf(stderr, "    Sizes don't match in dimension 0 (%d != %d)\n",this->length,theShape.GetSize0());
	    exit(EXIT_FAILURE);
	}
        ShapeBase<T>::operator=(theShape);
        return *this;
    }

    ShapeD1( VnusBase::markfntype markfn, const VnusInt card0 ):
     ShapeBase<T>( markfn, card0 )
    {
    }
    
        // Constructor for intialized arrays.
    ShapeD1( VnusBase::markfntype markfn, T *arr, const VnusInt asz, const VnusInt card0 ):
     ShapeBase<T>( markfn, card0 )
    {
        if( VNUS_UNLIKELY( asz != this->length ) ){
            fprintf(stderr,"*** ERROR -- \n");
            fprintf(stderr,"    Initializer array has %d  elms, but %d elms are required\n",asz, this->length );
            exit(1);
        }
	this->values = (T *) vnus_alloc_array( sizeof( T ), this->length, false );
	memcpy( this->values, arr, sizeof( T )*asz );
    }

public:
    T values[1];

    static inline ShapeD1<T> *create_shape( VnusBase::markfntype p_markfn, VnusInt card0 )
    {
	VnusInt elms = card0==0?0:(card0-1);
	ShapeD1<T> *res = new (elms*sizeof(T),false) ShapeD1<T>( p_markfn, card0 );
	return res;
    }

    static inline ShapeD1<T> *create_nulled_shape( VnusBase::markfntype p_markfn, VnusInt card0 )
    {
	VnusInt elms = card0==0?0:(card0-1);
	ShapeD1<T> *res = new (elms*sizeof(T),true) ShapeD1<T>( p_markfn, card0 );
	return res;
    }

    static inline ShapeD1<T> *create_filled_shape( VnusBase::markfntype p_markfn, T val, VnusInt card0 )
    {
	VnusInt elms = card0==0?0:(card0-1);
	ShapeD1<T> *res = new (elms*sizeof(T),false) ShapeD1<T>( p_markfn, card0 );
	for( VnusInt ix=0; ix<card0; ix++ ){
	    res->values[ix] = val;
	}
	return res;
    }
 
    static ShapeD1<T> *create_init_shape( VnusBase::markfntype p_markfn, T *val, VnusInt card0 )
    {
	VnusInt elms = card0==0?0:(card0-1);
	ShapeD1<T> *res = new (elms*sizeof(T),false) ShapeD1<T>( p_markfn, card0 );
	for( VnusInt ix=0; ix<card0; ix++ ){
	    res->values[ix] = val[ix];
	}
	return res;
    }
 
    inline T& operator() ( const VnusInt i0 )
    {
        return AccessChecked( i0 );
    }

    // Subscript operator for flat selection access.
    inline T& operator[]( const int i ) { return values[i]; }

    inline T& AccessChecked( const VnusInt i0 )
    {
        CheckIndex( i0 );
        return this->values[i0];
    }

    // Access to a shape element.
    // Similar to above, but these functions are not virtual.
    // Access to these functions is generated if it is known whether
    // the array is a shape.
    inline T& AccessNotChecked( const VnusInt i0 )
    {
        return this->values[i0];
    }

    inline void CheckIndex( const VnusInt i0 ) const
    {
	if( VNUS_UNLIKELY( ((UnsignedVnusInt) i0) >= (UnsignedVnusInt) this->length ) ){
	    VnusEventArrayBoundViolated();
	}
    }

    inline VnusInt GetSize0() const { return this->length; }

    inline VnusInt GetSize( const VnusInt dim ) const
    {
	switch( dim ){
	    case 0:
		return this->length;

	    default:
		runtime_error( "ShapeD1::GetSize: Illegal dimension" );
		return -1;

	}
    }

    inline T *GetBuffer() { return values; }

    void PrintInfo( FILE* out )
    {
        fprintf( out, "Shape[rank:1,length:%d]", this->length );
    }
    

    inline ShapeD1<T>* Fill (const T defaultValue)
    {
        return (ShapeD1<T>*)ShapeBase<T>::Fill(defaultValue);
    }

};

#endif
