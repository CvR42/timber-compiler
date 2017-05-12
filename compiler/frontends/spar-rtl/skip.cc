// File: skip.cc

#include <sys/types.h>
#include <unistd.h>
#include <vnusstd.h>
#include <shape.h>
#include "spar-rtl.h"

VnusLong Java_spar_compiler_File_skip( VnusInt hnd, VnusLong n )
{
    // Since lseek returns the offset from the start of the file,
    // and this function must return the actual number of bytes skipped,
    // and there is no separate function to tell the current position
    // in a file, abuse lseek to determine the current position.
    off_t base = lseek( hnd, (off_t) 0, SEEK_CUR );
    if( base<0 ){
	return -1;
    }
    off_t res = lseek( hnd, (off_t) n, SEEK_CUR );
    return (VnusLong) (res-base);
}
