#ifndef __VNUSTYPES_H__
#define __VNUSTYPES_H__

#include <complex>
#include <longlong.h>
typedef signed char VnusByte;
typedef short VnusShort;
typedef int VnusInt;
#if defined( __GNUC__ ) || defined(__INTEL_COMPILER)
__extension__ typedef long long int VnusLong;
#else
typedef signed_longlong VnusLong;
#endif
typedef float VnusFloat;
typedef double VnusDouble;
// CvR: The typedef below should work on GCC 3.0 and further, and be
// a lot faster than the C++ complex classes.
// TODO: construct the right test to take advantage of it.
//typedef _Complex double VnusComplex;
typedef std::complex<double> VnusComplex;
typedef bool VnusBoolean;
typedef char VnusChar;
typedef const char * VnusString;

/* The following types are only necessary to implement >>> */
typedef unsigned char UnsignedVnusByte;
typedef unsigned short UnsignedVnusShort;
typedef unsigned int UnsignedVnusInt;
#if defined( __GNUC__ ) || defined(__INTEL_COMPILER)
__extension__ typedef unsigned long long UnsignedVnusLong;
#else
typedef unsigned_longlong UnsignedVnusLong;
#endif

#endif
