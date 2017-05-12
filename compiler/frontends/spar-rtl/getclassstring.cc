// File: getclassstring.cc

#include <vnustypes.h>
#include <vnusstd.h>
#include <shape.h>
#include "getclassstring.h"

char *get_class_string( const VnusChar *nm, VnusInt len )
{
    // TODO: do unicode translation.
    size_t sz = (size_t) len;
    char *res = (char *) malloc( sz+1 );
    if( res == NULL ){
	return NULL;
    }
    memcpy( res, nm, sz );
    res[sz] = '\0';
    return res;
}
