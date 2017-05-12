// File: open.cc

#include <unistd.h>
#include <vnustypes.h>
#include <vnusstd.h>
#include <shape.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "spar-rtl.h"
#include "getfilestring.h"

// File open mode (rw-rw-rw-)
#define OPENMODE 0666

VnusInt Java_spar_compiler_File_openInput( VnusChar *nm, VnusInt len )
{
    char *nm_string = get_file_string( nm, len );

    if( nm_string == NULL ){
	return -1;
    }
    int res = open( nm_string, O_RDONLY, OPENMODE );
    free( nm_string );
    return (VnusInt) res;
}

VnusInt Java_spar_compiler_File_openOutput( VnusChar *nm, VnusInt len )
{
    char *nm_string = get_file_string( nm, len );

    if( nm_string == NULL ){
	return -1;
    }
    int res = open( nm_string, O_WRONLY|O_CREAT|O_TRUNC, OPENMODE );
    free( nm_string );
    return (VnusInt) res;
}

VnusInt Java_spar_compiler_File_openAppend( VnusChar *nm, VnusInt len )
{
    char *nm_string = get_file_string( nm, len );

    if( nm_string == NULL ){
	return -1;
    }
    int res = open( nm_string, O_WRONLY|O_CREAT|O_APPEND, OPENMODE );
    free( nm_string );
    return (VnusInt) res;
}
