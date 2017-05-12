// File: shaped0.h

#ifndef __SHAPED0_H__
#define __SHAPED0_H__

#include "polywrite.h"
#include "vnusstd.h"
#include "shapebase.h"
#include "outofmemory.h"
#include <stdio.h>
#include <string.h>
#include <new>

template<class T> class ShapeD0 : public ShapeBase<T>
{
private:
    // Assign one shape to another, but only if they are exactly alike.
    // This should not be used... I hope...
    ShapeD0<T>& operator= (const ShapeD0<T>& theShape)
    {
        if( this == &theShape ){
            return *this;
        }
        ShapeBase<T>::operator=(theShape);
        return *this;
    }

    inline ShapeD0( const ShapeD0<T>& theShape ):
     ShapeBase<T>( theShape )
    {
    }

    ShapeD0( VnusBase::markfntype markfn ):
     ShapeBase<T>( markfn, 1 )
    {
    }


public:
    T values[1];

    static inline ShapeD0<T> *create_shape( VnusBase::markfntype p_markfn )
    {
	ShapeD0<T> *res = new ShapeD0<T>( p_markfn );
	return res;
    }

    static inline ShapeD0<T> *create_nulled_shape( VnusBase::markfntype p_markfn )
    {
	ShapeD0<T> *res = new ShapeD0<T>( p_markfn );
	return res;
    }

    static inline ShapeD0<T> *create_filled_shape( VnusBase::markfntype p_markfn, T val )
    {
	ShapeD0<T> *res = new ShapeD0<T>( p_markfn );
	res->values[0] = val;
	return res;
    }

    inline T& operator() () { return AccessChecked(); }

    T& AccessChecked() { CheckIndex(); return AccessNotChecked(); }

    // For now the SetSize function does no communication.
    void SetSize()
    {}

    // Access to a shape element.

    inline T& AccessNotChecked()
    {
        return values[0];
    }

    // Subscript operator for flat selection access.
    inline T& operator[]( const int i ) { return values[i]; }

    inline void CheckIndex() const {}

    inline VnusInt Dim() const { return 0; }

    inline VnusInt GetSize( const VnusInt dim ) const
    {
        return 1;
    }

    inline T *GetBuffer() { return values; }

    void PrintInfo( FILE* out )
    {
        fprintf( out, "Shape[rank:0,length:1]" );
    }


        // Stuff from ShapeBase adapted for 0 dimensions.
    inline VnusInt GetLength() const { return 1; }

/*
 * Fill just allocated Shape with a default value (must be basic type).
 * returns 'this' so that it can be used in new expressions.
 */
    inline ShapeD0<T>* Fill (const T defaultValue)
    {
        return (ShapeD0<T>*)ShapeBase<T>::Fill(defaultValue);
    }

    void Print( FILE *out ) const
    {
        pm_write( out, this->values );
        fprintf( out, "\n" );
    }
};

#endif
