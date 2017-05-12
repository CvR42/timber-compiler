// File: shaped2.h

#ifndef __SHAPED2_H__
#define __SHAPED2_H__

#include "polywrite.h"
#include "vnusstd.h"
#include "shapebase.h"
#include "arrayboundviolated.h"

#include <stdio.h>
#include <string.h>

template<class T> class ShapeD2 : public ShapeBase<T>
{
private:
    const VnusInt dim0;
    const VnusInt dim1;

    inline ShapeD2( const ShapeD2<T>& theShape):
     ShapeBase<T>(theShape),dim0(theShape.dim0),dim1(theShape.dim1)
    {
    }

    ShapeD2( VnusBase::markfntype markfn, const VnusInt card0, const VnusInt card1 ):
     ShapeBase<T>( markfn, card0*card1 ), dim0(card0), dim1(card1)
    {
    }

        // Constructor for intialized arrays.
    ShapeD2( VnusBase::markfntype markfn, T *arr, const VnusInt asz, const VnusInt card0, const VnusInt card1 ):
     ShapeBase<T>( markfn, card0*card1 ), dim0(card0), dim1(card1)
    {
	assert( false );
    }

    // Assign one shape to another, but only if they are exactly alike.
    ShapeD2<T>& operator= (const ShapeD2<T>& theShape)
    {
        if( this == &theShape ){
            return *this;
        }
	if( VNUS_UNLIKELY( dim0 != theShape.GetSize0() ) ){
	    fprintf(stderr, "*** ERROR -- \n");
	    fprintf(stderr, "    Sizes don't match in dimension 0 (%d != %d)\n",dim0,theShape.GetSize0());
	    exit(1);
	}
	if( VNUS_UNLIKELY( dim1 != theShape.GetSize1() ) ){
	    fprintf(stderr, "*** ERROR -- \n");
	    fprintf(stderr, "    Sizes don't match in dimension 1 (%d != %d)\n",dim1,theShape.GetSize1());
	    exit(1);
	}
        ShapeBase<T>::operator=(theShape);
        return *this;
    }

public:
    T values[1];

    static inline ShapeD2<T> *create_shape( VnusBase::markfntype p_markfn, VnusInt d0, VnusInt d1 )
    {
	VnusInt elms = d0*d1;

	if( elms != 0 ){
	    elms--;
	}
	ShapeD2<T> *res = new (elms*sizeof(T),false) ShapeD2<T>( p_markfn, d0, d1 );

	return res;
    }

    static inline ShapeD2<T> *create_nulled_shape( VnusBase::markfntype p_markfn, VnusInt d0, VnusInt d1 )
    {
	VnusInt elms = d0*d1;

	if( elms != 0 ){
	    elms--;
	}
	ShapeD2<T> *res = new (elms*sizeof(T),true) ShapeD2<T>( p_markfn, d0, d1 );

	return res;
    }

    static inline ShapeD2<T> *create_filled_shape( VnusBase::markfntype p_markfn, T val, VnusInt d0, VnusInt d1 )
    {
	VnusInt elms = d0*d1;

	if( elms != 0 ){
	    elms--;
	}
	ShapeD2<T> *res = new (elms*sizeof(T),false) ShapeD2<T>( p_markfn, d0, d1 );

	for( VnusInt ix=0; ix<res->length; ix++ ){
	    res->values[ix] = val;
	}
	return res;
    }

    inline T& operator() ( const VnusInt i0, const VnusInt i1 )
    {
        return AccessChecked( i0, i1 );
    }

    // Subscript operator for flat selection access.
    inline T& operator[]( const int i ) { return values[i]; }

    inline T& AccessChecked( const VnusInt i0, const VnusInt i1 )
    {
        CheckIndex( i0, i1);
        return AccessNotChecked( i0, i1 );
    }

    // Access to a shape element.
    // Similar to above, but these functions are not virtual.
    // Access to these functions is generated if it is known whether
    // the array is a shape.
    inline T& AccessNotChecked( const VnusInt i0, const VnusInt i1 )
    {
        return values[i0*dim1+i1];
    }

    void CheckIndex( const VnusInt i0, const VnusInt i1 ) const
    {
	if(
	    (((UnsignedVnusInt) i0)>= (UnsignedVnusInt) dim0) ||
	    (((UnsignedVnusInt) i1)>= (UnsignedVnusInt) dim1)
	){
	    VnusEventArrayBoundViolated();
	}
    }

    inline VnusInt GetSize0() const { return dim0; }
    inline VnusInt GetSize1() const { return dim1; }

    inline VnusInt GetSize( const VnusInt dim ) const
    {
	if( dim == 0 ){
	    return dim0;
	}
	if( dim == 1 ){
	    return dim1;
	}
	runtime_error("ShapeD2::GetSize: Illegal dimension");
        return -1;
    }

    inline T *GetBuffer() { return values; }

    void PrintInfo( FILE* out )
    {
        fprintf( out, "Shape[rank:2,length:%d]", this->length );
    }

    inline ShapeD2<T>* Fill (const T defaultValue)
    {
        return (ShapeD2<T>*)ShapeBase<T>::Fill(defaultValue);
    }

};

#endif
