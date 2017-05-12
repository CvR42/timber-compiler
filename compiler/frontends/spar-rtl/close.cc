// File: close.cc

#include <unistd.h>
#include <vnustypes.h>
#include <vnusstd.h>
#include <shape.h>
#include "spar-rtl.h"

VnusBoolean Java_spar_compiler_File_close( VnusInt hnd )
{
    int res = close( hnd );
    return res == 0;
}
