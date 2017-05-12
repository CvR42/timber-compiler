// File: shapedn.h

#ifndef __SHAPEDN_H__
#define __SHAPEDN_H__

#include "polywrite.h"
#include "vnusstd.h"
#include "shapebase.h"
#include "arrayboundviolated.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <new>

template<class T, int rank> class ShapeDn : public ShapeBase<T>
{
private:
    VnusInt dims[rank];
    VnusInt weights[rank];

    inline ShapeDn( const ShapeDn<T,rank>& theShape):
     ShapeBase<T>(theShape)
    {
	for( int i=0; i<rank; i++ ){
	    dims[i] = theShape.dims[i];
	    weights[i] = theShape.weights[i];
	}
    }

    // Assign one shape to another, but only if they are exactly alike.
    ShapeDn<T,rank>& operator= (const ShapeDn<T,rank>& theShape)
    {
        if( this == &theShape ){
            return *this;
        }
        for( VnusInt d = 0; d<rank; d++ ){
            if( VNUS_UNLIKELY( GetSize(d) != theShape.GetSize(d) ) ){
                fprintf(stderr, "*** ERROR -- \n");
                fprintf(stderr, "    Sizes don't match in dimension %d (%d != %d)\n",d,GetSize(d),theShape.GetSize(d));
                exit(1);
            }
        }
        ShapeBase<T>::operator=( theShape );
        return *this;
    }

    ShapeDn( VnusBase::markfntype markfn, VnusInt length, va_list dn ):
     ShapeBase<T>( markfn, length )
    {
	for( int i=0; i<rank; i++ ){
	    dims[i] = va_arg( dn, VnusInt );
	}
	for( int i=1; i<rank; i++ ){
	    weights[i] = weights[i-1]*dims[i];
	}
    }

    ShapeDn( VnusBase::markfntype markfn, const VnusInt card0, ... ):
     ShapeBase<T>( markfn, 0 )
    {
	va_list cards;

	dims[0] = card0;
	weights[0] = 1;
	this->length = card0;

	va_start(cards, card0);
	for( int i=1; i<rank; i++ ){
	    dims[i] = va_arg(cards, VnusInt);
	    weights[i] = weights[i-1]*dims[i];
	    this->length *= dims[i];
	}
	va_end(cards);
	this->values = (T *) vnus_alloc_array( sizeof( T ), this->length, true );
    }

        // Constructor for intialized arrays.
    ShapeDn( VnusBase::markfntype markfn, T *arr, const VnusInt asz, const VnusInt card0, ... ):
     ShapeBase<T>( markfn, 0 )
    {
	assert( false );
    }

public:
    T values[1];

    static ShapeDn<T,rank> *create_shape( VnusBase::markfntype p_markfn, ... )
    {
	va_list cards;
	va_start( cards, p_markfn );
	VnusInt length = calculate_length( cards ); 
	va_end( cards );

	va_start( cards, p_markfn );
	ShapeDn<T,rank> *res = new (length*sizeof(T),false) ShapeDn<T,rank>( p_markfn, length, cards );
	va_end( cards );

	return res;
    }

    static ShapeDn<T,rank> *create_nulled_shape( VnusBase::markfntype p_markfn, ... )
    {
	va_list cards;
	va_start( cards, p_markfn );
	VnusInt length = calculate_length( cards ); 
	va_end( cards );

	va_start( cards, p_markfn );
	ShapeDn<T,rank> *res = new (length*sizeof(T),true) ShapeDn<T,rank>( p_markfn, length, cards );
	va_end( cards );

	return res;
    }

    static ShapeDn<T,rank> *create_filled_shape( VnusBase::markfntype p_markfn, T val, ... )
    {
	va_list cards;
	va_start( cards, val );
	VnusInt length = calculate_length( cards ); 
	va_end( cards );

	va_start( cards, val );
	ShapeDn<T,rank> *res = new (length*sizeof(T),false) ShapeDn<T,rank>( p_markfn, length, cards );
	va_end( cards );

	for( VnusInt ix=0; ix<length; ix++ ){
	    res->values[ix] = val;
	}
	return res;
    }

    T& operator() ( const VnusInt i0, ... ) const
    {
        va_list args;
	VnusInt index[rank];

        va_start( args, i0 );
	grabVA( index, i0, args );
        va_end( args );
        CheckIndex( index );
        T& result = AccessNotChecked( index );
        return result;
    }

    // Subscript operator for flat selection access.
    inline T& operator[]( const int i ) { return values[i]; }

    T& AccessChecked( const VnusInt i0, ... ) const
    {
        va_list args;
	VnusInt index[rank];

        va_start( args, i0 );
	grabVA( index, i0, args );
        va_end( args );
        CheckIndex( index );
        T& result = AccessNotChecked( index );
        return result;
    }

    inline T& AccessNotChecked( VnusInt const *index ) const
    {
	VnusInt ix = 0;

	for( int i=0; i<rank; i++ ){
	    ix += index[i]*weights[i];
	}
        return this->values[ix];
    }

    inline VnusInt GetSize( const VnusInt dim ) const
    {
	if( VNUS_UNLIKELY( dim>=rank ) ){
            fprintf(stderr,"*** ERROR -- \n");
            fprintf(stderr,"    GetSize dimension out of range: dim=%ld, rank=%ld\n", (long int) dim, (long int) rank );
            exit(1);
	}
        return dims[dim];
    }

    inline T *GetBuffer() { return values; }

    void PrintInfo( FILE* out )
    {
        fprintf(out,"Shape[rank:%d,length:%d]",rank,this->length);
    }

    inline ShapeDn<T,rank>* Fill (const T defaultValue)
    {
        return (ShapeDn<T,rank>*)ShapeBase<T>::Fill(defaultValue);
    }

private:
    void CheckIndex( VnusInt const *index ) const
    {
	for( int i=0; i<rank; i++ ){
	    if( VNUS_UNLIKELY( ((UnsignedVnusInt) index[i])>= (UnsignedVnusInt) dims[i] ) ){
		VnusEventArrayBoundViolated();
	    }
	}
    }

    static VnusInt calculate_length( va_list dims )
    {
	VnusInt length = 1;

	for( unsigned int i=0; i<rank; i++ ){
	    VnusInt val = va_arg( dims, VnusInt );

	    length *= val;
	}
	return length;
    }

    static void grabVA( VnusInt *index, VnusInt i0, va_list iEtc )
    {
	index[0] = i0;

	for( int i=1; i<rank; i++ ){
	    index[i] = va_arg(iEtc,VnusInt);
	}
    }
};

#endif
