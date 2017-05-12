// File: fileexists.cc

#include <errno.h>
#include <unistd.h>
#include <vnustypes.h>
#include <vnusstd.h>
#include <shape.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "spar-rtl.h"
#include "getfilestring.h"

// File open mode (rw-rw-rw-)
#define OPENMODE 0666

// Return 1 if the file exists and can be checked, 0 if the file
// does not exist, or -1 if it is a security violation to check
// wether this file exists.
VnusInt Java_spar_compiler_File_exists( VnusChar *nm, VnusInt len )
{
    char *nm_string = get_file_string( nm, len );
    struct stat buf;
    int res = stat( nm_string, &buf );
    if( res == 0 ){
	return 1;	// Yep, file exists.
    }
    if( errno == EACCES ){
	return -1;	// Permission denied.
    }
    return 0;
}
