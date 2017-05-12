// File: spar-rtl.h
//
// Header file for the runtime library of Spar.

#ifndef INC_SPAR_RTL_H
#define INC_SPAR_RTL_H

#include <math.h>
#include <string.h>
#include <unistd.h>
#include "vnusbaseclass.h"
#include "serialisation.h"

// Internalization routines.
inline VnusInt Java_spar_compiler_Internalize_strlen( VnusString &s ){ return (VnusInt) strlen( s ); }
extern void Java_spar_compiler_Internalize_unicodify(
 VnusChar *buf,
 VnusInt length,
 VnusString &s
);

extern VnusLong Java_java_lang_System_currentTimeMillis();
extern void *Java_java_lang_System_initProperties( void *ths );

// Given an object, compute its hash code. The function should always return
// the same value if a call to 'equals()' returns true, and 'the converse
// it not necessaruly true'. This is about the dumbest (and
// least efficient) implementation which is legal. 
inline VnusInt Java_java_lang_Object_hashCode( void *ths ) {
    (void) ths;
    return 0;
}

extern void *Java_java_lang_Throwable_fillInStackTrace( void *ths );

// File I/O routines. They use the Unix files at open(2) level
extern VnusBoolean Java_spar_compiler_File_write( VnusInt hnd, VnusByte *buf, VnusInt off, VnusInt len );
extern VnusInt Java_spar_compiler_File_read( VnusInt hnd, VnusByte *buf, VnusInt off, VnusInt len );
inline VnusInt Java_spar_compiler_File_readByte( VnusInt hnd ) 
{
    signed char c;

    ssize_t res = read( (int) hnd, &c, 1 );
    if( res<=0 ){
	return 130;
    }
    return (VnusInt) c;
}

extern VnusInt Java_spar_compiler_File_openInput( VnusChar *nm, VnusInt len );
extern VnusInt Java_spar_compiler_File_openOutput( VnusChar *nm, VnusInt len );
extern VnusInt Java_spar_compiler_File_openAppend( VnusChar *nm, VnusInt len );
extern VnusBoolean Java_spar_compiler_File_close( VnusInt hnd );
extern VnusLong Java_spar_compiler_File_skip( VnusInt hnd, VnusLong n );
extern VnusInt Java_spar_compiler_File_available( VnusInt hnd );
extern VnusInt Java_spar_compiler_File_exists( VnusChar *nm, VnusInt len );
extern VnusInt Java_spar_compiler_File_isFile( VnusChar *nm, VnusInt len );
extern VnusInt Java_spar_compiler_File_isDirectory( VnusChar *nm, VnusInt len );
extern VnusBoolean Java_spar_compiler_File_readable( VnusChar *nm, VnusInt len );
extern VnusBoolean Java_spar_compiler_File_writable( VnusChar *nm, VnusInt len );

extern void *Java_java_lang_Class_newInstance( void *ths );

// For the moment we simply return null
extern void *Java_java_lang_Class_getPrimitiveClass( VnusString ths );

extern VnusInt Java_spar_compiler_Introspection_getClassId( void *obj );
extern VnusInt Java_spar_compiler_Introspection_ClassIdFromString1( VnusChar *nm, VnusInt len );
extern VnusString Java_spar_compiler_Introspection_getClassName( VnusInt id );
extern VnusInt Java_spar_compiler_Introspection_getModifiers( VnusInt id );
extern VnusBoolean Java_spar_compiler_Introspection_isInterface( VnusInt id );
extern VnusBoolean Java_spar_compiler_Introspection_isPrimitive( VnusInt id );

inline void Java_kaffe_lang_MemoryAdvice_register0( void *ths ){
    (void) ths;
    // The exact same implementation as in kaffe: nothing :-)
}

inline void Java_java_lang_Runtime_exitInternal( void *, VnusInt status ){
    exit( status );
}
inline void Java_spar_compiler_DefaultExceptionHandler_exit( VnusString s ){
    fprintf( stderr, "Unhandled exception '%s'; program aborted\n", s );
    exit( 2 );
}

// Some arithmetic operators.
inline VnusDouble Java_java_lang_Math_abs( VnusDouble a ) { return fabs( a ); }
inline VnusDouble Java_java_lang_Math_sin( VnusDouble a ) { return sin( a ); }
inline VnusDouble Java_java_lang_Math_cos( VnusDouble a ) { return cos( a ); }
inline VnusDouble Java_java_lang_Math_tan( VnusDouble a ) { return tan( a ); }
inline VnusDouble Java_java_lang_Math_asin( VnusDouble a ) { return asin( a ); }
inline VnusDouble Java_java_lang_Math_acos( VnusDouble a ) { return acos( a ); }
inline VnusDouble Java_java_lang_Math_atan( VnusDouble a ) { return atan( a ); }
inline VnusDouble Java_java_lang_Math_atan2( VnusDouble a, VnusDouble b ) { return atan2( a, b ); }
inline VnusDouble Java_java_lang_Math_ceil( VnusDouble a ) { return ceil( a ); }
inline VnusDouble Java_java_lang_Math_floor( VnusDouble a ) { return floor( a ); }
inline VnusDouble Java_java_lang_Math_exp( VnusDouble a ) { return exp( a ); }
inline VnusDouble Java_java_lang_Math_log( VnusDouble a ) { return log( a ); }
inline VnusDouble Java_java_lang_Math_pow( VnusDouble a, VnusDouble b ) { return pow( a, b ); }
inline VnusDouble Java_java_lang_Math_sqrt( VnusDouble a ) { return sqrt( a ); }
inline VnusDouble Java_java_lang_Math_IEEEremainder( VnusDouble x, VnusDouble y ) { return fmod( x, y ); }
extern VnusDouble Java_java_lang_Math_rint( const VnusDouble a );

#if defined( __GNUC__ ) || defined( __INTEL_COMPILER )

// Ditto for complex values
inline VnusComplex Java_java_lang_Complex_sin( VnusComplex a ) { return sin( a ); }
inline VnusComplex Java_java_lang_Complex_cos( VnusComplex a ) { return cos( a ); }
//inline VnusComplex Java_java_lang_Complex_tan( VnusComplex a ) { return tan( a ); }
inline VnusComplex Java_java_lang_Complex_sqrt( VnusComplex a ) { return sqrt( a ); }
inline VnusComplex Java_java_lang_Complex_exp( VnusComplex a ) { return exp( a ); }
inline VnusComplex Java_java_lang_Complex_log( VnusComplex a ) { return log( a ); }
inline VnusDouble Java_java_lang_Complex_abs( VnusComplex a ) { return abs( a ); }
inline VnusDouble Java_java_lang_Complex_arg( VnusComplex a ) { return arg( a ); }
inline VnusDouble Java_java_lang_Complex_norm( VnusComplex a ) { return norm( a ); }
inline VnusComplex Java_java_lang_Complex_polar( VnusDouble a, VnusDouble b ) { return std::polar( a, b ); }
inline VnusComplex Java_java_lang_Complex_pow( VnusComplex a, VnusComplex b ) { return pow( a, b ); }


#endif

inline VnusComplex Java_java_lang_Complex_conj( VnusComplex a ) { return conj( a ); }
inline VnusDouble Java_java_lang_Complex_real( VnusComplex a ) { return real( a ); }
inline VnusDouble Java_java_lang_Complex_imag( VnusComplex a ) { return imag( a ); }

// Conversion routines.
extern VnusDouble Java_spar_compiler_Double_valueOf1( VnusChar *s, VnusInt slen, VnusChar *msg, VnusInt msgbugfsize );
extern VnusLong Java_java_lang_Double_doubleToLongBits( VnusDouble v );
extern VnusDouble Java_java_lang_Double_longBitsToDouble( VnusLong v );
extern VnusInt Java_java_lang_Float_floatToIntBits( VnusFloat v );
extern VnusFloat Java_java_lang_Float_intBitsToFloat( VnusInt v );

// String conversion routines
extern void Java_spar_compiler_Double_formatDoubleString(
 VnusChar *cbuf,
 VnusInt buflen,
 VnusDouble val,
 VnusInt precision
);

// Argv routines
extern VnusInt Java_spar_compiler_Argv_getArgc();
extern VnusString Java_spar_compiler_Argv_getArg( VnusInt i );

extern void Java_spar_compiler_GC_invokeMarkingMethod( VnusBase *obj );
extern void Java_spar_compiler_GC_invokeFinalize( VnusBase *obj );

// Should be declared by all generated Vnus programs.
extern VnusInt numberOfProcessors;

// spar.lang.DataParallel methods
inline VnusInt Java_spar_lang_DataParallel_getNumberOfProcessors(){
    return numberOfProcessors;
}

#endif
