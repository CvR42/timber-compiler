#include <math.h>
#include "spar-rtl.h"

// This implements java.lang.Math.rint().
VnusDouble Java_java_lang_Math_rint( const VnusDouble x )
{
    // "If x is [...] NaN or an infinity, x is returned".
    // (Note that this also covers 0.0, but that's ok.)
    if( x == 2.0*x ){
	return x;
    }
    double ip;
    // We're not interested in the fraction.
    (void) modf( x, &ip );
    if( ip == x ){
	return x;
    }
    double ip1;
    double ip2;
    if( ip>x ){
	ip1 = ip-1.0;
	ip2 = ip;
    }
    else {
	ip1 = ip;
	ip2 = ip+1.0;
    }
    // We now know that ip1 and ip2 are integer, and that
    // ip1<x<ip2
    double delta1 = x-ip1;	// Presumably >=0.
    double delta2 = ip2-x;
    if( delta1 == delta2 ){
	// "If x is equally close to two whole numbers, the even
	// whole number is returned."
	if( fmod( ip1, 2.0 )<fmod( ip2, 2.0 ) ){
	    return ip1;
	}
	else {
	    return ip2;
	}
    }
    // "The rounding is to the closest whole number."
    if( delta1>delta2 ){
	return ip2;
    }
    return ip1;
}
