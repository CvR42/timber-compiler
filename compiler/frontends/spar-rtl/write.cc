// File: write.cc

#include <unistd.h>
#include <vnustypes.h>
#include <vnusstd.h>
#include <shape.h>
#include "spar-rtl.h"

VnusBoolean Java_spar_compiler_File_write(
    VnusInt hnd,
    VnusByte *buf,
    VnusInt off,
    VnusInt len
)
{
    ssize_t res = write( hnd, buf+off, len );
    return (res != -1);
}
