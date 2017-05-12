// File: dfmt.cc
//

#include <vnustypes.h>
#include <vnusstd.h>
#include "spar-rtl.h"
#include <stdlib.h>
#include <assert.h>

#define MAXNUMLEN 100

// Write a string representation of the given double into 'cbuf'.
// The string is terminated with a '\0' so that we can determine the length
// of the resulting string in Java. It works here since the real string
// cannot contain a '\0'.
void Java_spar_compiler_Double_formatDoubleString(
 VnusChar *cbuf,
 VnusInt buflen,
 VnusDouble val,
 VnusInt precision
) 
{
    char buf[MAXNUMLEN];
    char *s = buf;
    int k;
    VnusLong valbits = Java_java_lang_Double_doubleToLongBits(val);

    if(
	val<0.0 ||
	valbits == Java_java_lang_Double_doubleToLongBits(-0.0)
    ){
	val = -val;
	*s++ = '-';
    }

    /* Print in normal or 'scientific' form according to value */
    if (val == 0.0 || (val >= 1.0e-3 && val < 1.0e7)) {
	static const double powTen[] = {
	  1.0e0, 1.0e1, 1.0e2, 1.0e3, 1.0e4, 1.0e5, 1.0e6
	};

	/* Account for precision digits ahead of the decimal point */
	for (k = 6; k >= 0 && val < powTen[k]; k--);
	precision -= k + 1;

	/* Print in decimal notation */
	sprintf(s, "%.*f", (int) precision, (double) val);

	/* Remove trailing zeroes after the decimal point */
	for (k = strlen(buf) - 1; buf[k] == '0' && buf[k - 1] != '.'; k--) {
	    buf[k] = '\0';
	}
    }
    else {
	char *t, *eptr, *eval;

	/* Print in exponential notation */
	sprintf(s, "%.*E", (int) precision - 1, (double) val);

	/* Find the exponent */
	eptr = strchr(buf, 'E');
	assert(eptr != NULL);

	/* Remove a '+' sign, but leave a '-' sign */
	switch (eptr[1]) {
	    case '-':
		eval = &eptr[2];
		break;

	    case '+':			/* remove '+' */
		memmove(eptr + 1, eptr + 2, strlen(eptr + 1));
		/* fall through */

	    default:			/* shouldn't ever happen */
		eval = &eptr[1];
		break;
	}

	/* Remove leading zeroes in the exponent, if any */
	for( t = eval; t[0] == '0' && t[1] != '\0'; t++ );
	memmove( eval, t, strlen(t) + 1 );

	/* Remove trailing zeroes after the decimal point */
	for( t = eptr - 1; *t == '0' && t[-1] != '.'; t-- );
	memmove( t + 1, eptr, strlen(eptr) + 1 );
    }
    const char *p = buf;
    VnusInt len = (VnusInt) strlen( p );
    if( len+1>buflen ){
	fprintf(
	    stderr,
	    "formatDoubleString: buffer array too short: I got %ld byts, I need %ld\n",
	    (long) buflen,
	    (long) len+1
	);
	exit( 2 );
    }
    VnusInt ix = 0;
    while( ix<len ){
	cbuf[ix++] = *p++;
    }
    cbuf[ix] = '\0';
}
