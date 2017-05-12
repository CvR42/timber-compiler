// File: shapebase.h

#ifndef __SHAPEBASE_H__
#define __SHAPEBASE_H__

#include <stdio.h>		// For printing methods

#include "polywrite.h"
#include "vnusstd.h"
#include "vnusbaseclass.h"
#include "vnustypes.h"

/*
  Base class for 0-, 1-, 2-, 3- and n-dimensional shapes.
  */
template<class T> class ShapeBase: public VnusBase
{
private:
    ShapeBase<T>& operator= (const ShapeBase<T>& theShape)
    {
	assert( false );
    }

    ShapeBase( const ShapeBase<T>& theShape ):
     VnusBase(),
     length(theShape.length)
    {
	assert( false );
    }


protected:
    const markfntype markfn;

    ShapeBase( markfntype p_markfn, VnusInt p_size ):
     VnusBase(), markfn(p_markfn), length(p_size)
    {}

public:
    const VnusInt length;

    inline VnusInt GetLength() const { return length; }

    virtual T* GetBuffer() = 0;

    inline markfntype getShapeMarkfn() const { return markfn; }

    inline bool isShape() const { return true; }

    void Print( FILE *out ) const
    {
        for( VnusInt i = 0; i<length; i++ ){
            pm_write(out,values[i]);
            fprintf(out," ");
        }
        fprintf(out,"\n");
    }
};

inline VnusBase *vnus_get_shape_buffer( VnusBase *arr )
{
    ShapeBase<int> *arr1 = (ShapeBase<int> *) arr;
    return (VnusBase *) arr1->GetBuffer();
}

inline VnusInt vnus_get_shape_length( VnusBase *arr )
{
    ShapeBase<int> *arr1 = (ShapeBase<int> *) arr;
    return arr1->length;
}
#endif
