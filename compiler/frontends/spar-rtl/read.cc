// File: read.cc

#include <unistd.h>
#include <vnustypes.h>
#include <vnusstd.h>
#include <shape.h>
#include "spar-rtl.h"

VnusInt Java_spar_compiler_File_read(
    VnusInt hnd,
    VnusByte *buf,
    VnusInt off,
    VnusInt len
)
{
    ssize_t res = read( hnd, buf+off, len );
    return (VnusInt) res;
}
