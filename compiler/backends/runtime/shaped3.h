// File: shaped3.h

#ifndef __SHAPED3_H__
#define __SHAPED3_H__

#include "polywrite.h"
#include "vnusstd.h"
#include "shapebase.h"
#include "arrayboundviolated.h"

#include <stdio.h>
#include <string.h>

template<class T> class ShapeD3 : public ShapeBase<T>
{
private:
    const VnusInt dim0;
    const VnusInt dim1;
    const VnusInt dim2;

    inline ShapeD3( const ShapeD3<T>& theShape):
     ShapeBase<T>(theShape),dim0(theShape.dim0),dim1(theShape.dim1),dim2(theShape.dim2)
    {
    }

    // Assign one shape to another, but only if they are exactly alike.
    ShapeD3<T>& operator= (const ShapeD3<T>& theShape)
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
	if( VNUS_UNLIKELY( dim2 != theShape.GetSize2() ) ){
	    fprintf(stderr, "*** ERROR -- \n");
	    fprintf(stderr, "    Sizes don't match in dimension 2 (%d != %d)\n",dim2,theShape.GetSize2());
	    exit(1);
	}
        ShapeBase<T>::operator=(theShape);
        return *this;
    }

    ShapeD3( VnusBase::markfntype markfn, const VnusInt card0, const VnusInt card1, const VnusInt card2 ):
     ShapeBase<T>( markfn, card0*card1*card2 ), dim0(card0), dim1(card1), dim2(card2)
    {
    }

        // Constructor for intialized arrays.
    ShapeD3( VnusBase::markfntype markfn, T *arr, const VnusInt asz, const VnusInt card0, const VnusInt card1, const VnusInt card2 ):
     ShapeBase<T>( markfn, card0*card1*card2 ), dim0(card0), dim1(card1), dim2(card2)
    {
        if( VNUS_UNLIKELY( asz != this->length ) ){
            fprintf(stderr,"*** ERROR -- \n");
            fprintf(stderr,"    Initializer array has %d  elms, but %d elms are required\n",asz,this->length);
            exit(1);
        }
	this->values = (T *) vnus_alloc_array( sizeof( T ), this->length, false );
	memcpy( this->values, arr, sizeof( T )*asz );
    }

public:
    T values[1];

    static inline ShapeD3<T> *create_shape( VnusBase::markfntype p_markfn, VnusInt d0, VnusInt d1, VnusInt d2 )
    {
	VnusInt elms = d0*d1*d2;

	if( elms != 0 ){
	    elms--;
	}
	ShapeD3<T> *res = new (elms*sizeof(T),false) ShapeD3<T>( p_markfn, d0, d1, d2 );

	return res;
    }

    static inline ShapeD3<T> *create_nulled_shape( VnusBase::markfntype p_markfn, VnusInt d0, VnusInt d1, VnusInt d2 )
    {
	VnusInt elms = d0*d1*d2;

	if( elms != 0 ){
	    elms--;
	}
	ShapeD3<T> *res = new (elms*sizeof(T),true) ShapeD3<T>( p_markfn, d0, d1, d2 );

	return res;
    }

    static inline ShapeD3<T> *create_filled_shape( VnusBase::markfntype p_markfn, T val, VnusInt d0, VnusInt d1, VnusInt d2 )
    {
	VnusInt elms = d0*d1*d2;

	if( elms != 0 ){
	    elms--;
	}
	ShapeD3<T> *res = new (elms*sizeof(T),false) ShapeD3<T>( p_markfn, d0, d1, d2 );

	for( VnusInt ix=0; ix<res->length; ix++ ){
	    res->values[ix] = val;
	}
	return res;
    }

    inline T& operator() ( const VnusInt i0, const VnusInt i1, const VnusInt i2 )
    {
        return AccessChecked( i0, i1, i2 );
    }

    // Subscript operator for flat selection access.
    inline T& operator[]( const int i ) { return values[i]; }

    inline T& AccessChecked( const VnusInt i0, const VnusInt i1, const VnusInt i2 )
    {
        CheckIndex( i0, i1, i2 );
        return AccessNotChecked( i0, i1, i2 );
    }

    // Access to a shape element.
    // Similar to above, but these functions are not virtual.
    // Access to these functions is generated if it is known whether
    // the array is a shape.
    inline T& AccessNotChecked( const VnusInt i0, const VnusInt i1, const VnusInt i2 )
    {
        return values[(i0*dim1+i1)*dim2+i2];
    }

    inline void CheckIndex( const VnusInt i0, const VnusInt i1, const VnusInt i2 ) const
    {
	if(
	    VNUS_UNLIKELY(
		(((UnsignedVnusInt) i0)>= (UnsignedVnusInt) dim0) ||
		(((UnsignedVnusInt) i1)>= (UnsignedVnusInt) dim1) ||
		(((UnsignedVnusInt) i2)>= (UnsignedVnusInt) dim2)
	    )
	){
	    VnusEventArrayBoundViolated();
	}
    }

    inline VnusInt GetSize0() const { return dim0; }
    inline VnusInt GetSize1() const { return dim1; }
    inline VnusInt GetSize2() const { return dim2; }

    inline VnusInt GetSize( const VnusInt dim ) const
    {
	switch( dim ){
	    case 0:
		return dim0;

	    case 1:
		return dim1;

	    case 2:
		return dim2;

	    default:
                runtime_error("ShapeD3::GetSize: Illegal dimension");
	}
        return -1;
    }

    inline T *GetBuffer() { return values; }

    void PrintInfo( FILE* out )
    {
        fprintf( out, "Shape[rank:3,length:%d]",this->length );
    }


    inline ShapeD3<T>* Fill (const T defaultValue)
    {
        return (ShapeD3<T>*)ShapeBase<T>::Fill(defaultValue);
    }

};

#endif
