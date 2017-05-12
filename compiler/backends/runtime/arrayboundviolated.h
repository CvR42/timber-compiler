// File: arrayboundviolated.h

#ifndef INC_ARRAYBOUNDVIOLATED_H
#define INC_ARRAYBOUNDVIOLATED_H

extern void VnusEventArrayBoundViolated();

inline VnusBoolean VnusIsBoundViolated(VnusInt val, VnusInt bound)
{
    return ((UnsignedVnusInt) val)>=(UnsignedVnusInt) bound;
}

inline VnusInt VnusCheckBound( VnusInt val, VnusInt bound )
{
    if( VnusIsBoundViolated( val, bound ) ){
	VnusEventArrayBoundViolated();
    }
    return val;
}

inline VnusBoolean VnusIsUpperBoundViolated(VnusInt val, VnusInt bound)
{
    return val>=bound;
}

inline VnusInt VnusUpperCheckBound( VnusInt val, VnusInt bound )
{
    if( VnusIsUpperBoundViolated( val, bound ) ){
	VnusEventArrayBoundViolated();
    }
    return val;
}

inline VnusBoolean VnusIsLowerBoundViolated(VnusInt val)
{
    return val<0;
}

inline VnusInt VnusLowerCheckBound( VnusInt val)
{
    if( VnusIsLowerBoundViolated( val ) ){
	VnusEventArrayBoundViolated();
    }
    return val;
}

#endif
