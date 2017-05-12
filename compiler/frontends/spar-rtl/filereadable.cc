// File: filereadable.cc

#include <unistd.h>
#include <vnustypes.h>
#include <vnusstd.h>
#include <shape.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "spar-rtl.h"
#include "getfilestring.h"

// Return true iff the file can be read.
VnusBoolean Java_spar_compiler_File_readable( VnusChar *nm, int len )
{
    char *nm_string = get_file_string( nm, len );
    int res = access( nm_string, R_OK );
    return res == 0;
}
