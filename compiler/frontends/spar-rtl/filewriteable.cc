// File: filewriteable.cc

#include <unistd.h>
#include <vnustypes.h>
#include <vnusstd.h>
#include <shape.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "spar-rtl.h"
#include "getfilestring.h"

// Return true iff the file can be written.
VnusBoolean Java_spar_compiler_File_writeable( VnusChar *nm, VnusInt len )
{
    char *nm_string = get_file_string( nm, len );
    int res = access( nm_string, W_OK );
    return res == 0;
}
