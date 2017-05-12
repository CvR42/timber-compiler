// File: available.cc

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vnusstd.h>
#include <shape.h>
#include "spar-rtl.h"

VnusInt Java_spar_compiler_File_available( VnusInt hnd )
{
    off_t pos = lseek( hnd, (off_t) 0, SEEK_CUR );
    if( pos>=0 ) {
	struct stat statbuf;

	if( (fstat( hnd, &statbuf ) == 0) ){
	    if( S_ISREG( statbuf.st_mode ) ){
		return (VnusInt) (statbuf.st_size - pos);
	    }
	}
    }
    return (VnusInt) 1;
}
