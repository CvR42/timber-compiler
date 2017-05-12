// File: unicodify.cc

#include <vnusstd.h>
#include <shape.h>
#include <string.h>
#include "spar-rtl.h"

// Given a buffer 'buf' and a string 's', fill 'buf' with the string
// 's' decoded according to the Java unicoding standard.
// 'length' is the size of 'buf', and should match exactly with the
// length we arrive at.
void Java_spar_compiler_Internalize_unicodify(
 VnusChar *buf,
 VnusInt length,
 VnusString &s
)
{
    assert( s != NULL );
    const char *p = s;
    VnusInt pos = 0;

    while( *p != '\0' ){
	if( *p == '\\' ){
	    p++;
	    int c = *p++;
	    switch( c ){
		// See JLS 3.10.6 for a list of escape sequences

		case 'b':
		    buf[pos++] = 0x08;
		    break;
		    
		case 't':
		    buf[pos++] = '\t';
		    break;
		    
		case 'n':
		    buf[pos++] = '\n';
		    break;

		case 'f':
		    buf[pos++] = 0x0c;
		    break;

		case 'r':
		    buf[pos++] = '\r';
		    break;
		    
		case 'u':
		{
		    // Skip any further 'u's
		    while( *p == 'u' ) p++;

		    char hexcode[5];
		    memset( hexcode, '\0', 5 );
		    hexcode[0] = *p++;
		    if( hexcode[0] != '\0' ){
			hexcode[1] = *p++;
			if( hexcode[1] != '\0' ){
			    hexcode[2] = *p++;
			    if( hexcode[2] != '\0' ){
				hexcode[3] = *p++;
			    }
			}
		    }
		    hexcode[4] = '\0';
		    long int n = strtol( hexcode, NULL, 16 );
		    buf[pos++] = (unsigned short int) n;
		    break;
		}
		    
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		{
		    char octcode[4];
		    memset( octcode, '\0', 4 );
		    octcode[0] = *p++;
		    if( octcode[0]>='0' && octcode[0]<='7' ){
			octcode[1] = *p++;
			if( octcode[1]>='0' && octcode[1]<='7' && octcode[0]<='3' ){
			    octcode[2] = *p++;
			}
		    }
		    long int n = strtol( octcode, NULL, 8 );
		    buf[pos++] = (unsigned short int) n;
		    break;
		}

		case '"':
		case '\'':
		case '\\':
		default:
		    // Ordinary escaped character, just put it in
		    buf[pos++] = c;
		    break;
	    }
	}
	else {
	    // Ordinary character, do nothing special.
	    buf[pos++] = *p++;
	}
    }
    if( pos>length ){
	fprintf( stderr, "unicodify out of sync: size should be %ld, but is %ld\n", (long int) length, (long int) pos );
    }
}
