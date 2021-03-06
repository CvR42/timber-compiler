// File: isDirectory.cc

#include <errno.h>
#include <unistd.h>
#include <vnustypes.h>
#include <vnusstd.h>
#include <shape.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "spar-rtl.h"
#include "getfilestring.h"

// Return 1 if the file exists and is a directory , 0 if the file
// does not exist, or -1 if it is a security violation to check
// wether this file is a directory.
VnusInt Java_spar_compiler_File_isDirectory( VnusChar *nm, VnusInt len )
{
    char *nm_string = get_file_string( nm, len );
    struct stat buf;
    int res = stat( nm_string, &buf );
    if( res == 0 ){
	// File exists. Now see wether it is an ordinary file.
	return S_ISDIR(buf.st_mode);	
    }
    if( errno == EACCES ){
	return -1;	// Permission denied.
    }
    return 0;
}
