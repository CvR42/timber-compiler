// File: valueofdouble.c
//

#include <vnustypes.h>
#include <vnusstd.h>
#include "spar-rtl.h"
#include <string.h>
#include <errno.h>

#define MAXNUMLEN 200
#define HAVE_STRTOD

VnusDouble Java_spar_compiler_Double_valueOf1( VnusChar *s, VnusInt slen, VnusChar *msgbuf, VnusInt msgbufsize )
{
    double value;
    char buf[MAXNUMLEN];
    const char* msg = "Malformed float/double";
    msgbuf[0] = '\0';

    // First try to copy the string with the double in it into 'buf'.
    if( slen>=MAXNUMLEN ){
	msg = "Float/double string too long";
	goto bail;
    }
    memcpy( buf, s, slen );
    buf[slen] = '\0';

#if defined(HAVE_STRTOD)
    char* endbuf;
    value = strtod(buf, &endbuf);
    while( *endbuf != 0 ){
	switch (*endbuf) {
	    case ' ':
	    case '\t':
	    case '\n':
	    case '\r':      /* Ignore whitespace */
	    case 'F':
	    case 'f':       /* Ignore float suffix. */
	    case 'D':
	    case 'd':       /* Ignore double suffix. */
		endbuf++;
		break;

	    default:
		goto bail;
	}
    }
    /* don't return 0 if string was empty */
    if (endbuf == buf) {
	msg = "empty string";
	goto bail;
    }
    if (errno == ERANGE) {
	if (value == HUGE_VAL || value == -HUGE_VAL) {
	    msg = "Overflow";
	    goto bail;
	}
	if (value == 0.0) {
	    msg = "Underflow";
	    goto bail;
	}
    }
#else
    /* Fall back on old atof - no error checking */
    value = atof(buf);
#endif

    return value;

bail:;
    // There was an error, stuff the message in 'msg' in 'msgbuf'.
    size_t msglen = strlen( msg );
    unsigned int ix;
    for( ix=0; ix<msglen; ix++ ){
	if( (VnusInt) ix>msgbufsize ){
	    break;
	}
	msgbuf[ix] = msg[ix];
    }
    if( (VnusInt) ix<msgbufsize ){
	msgbuf[ix] = '\0';
    }
    return 0;
}
