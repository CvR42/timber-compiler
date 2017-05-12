#ifndef __POLYWRITE_H__
#define __POLYWRITE_H__
/*
 * Polymorphic write functions to replace usage of ostream.
 */

#include "vnustypes.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

/*
 * Write single element of a certain type to file with handle fileHandle.
 */

inline void pm_write(FILE* fileHandle, VnusByte elt)
{
    fprintf(fileHandle,"%d", (int)elt);
}

inline void pm_write(FILE* fileHandle, VnusShort elt)
{
    fprintf(fileHandle,"%d", (int)elt);
}

inline void pm_write(FILE* fileHandle, VnusInt elt)
{
    fprintf(fileHandle,"%d", (int)elt);
}

inline void pm_write(FILE* fileHandle, VnusLong elt)
{
#ifdef __GNUC__
    __extension__ fprintf( fileHandle, "%lld", elt );
#else
  printf("ERROR: pm_write(VnusLong) not implemented.\n");
  assert(0); exit(1);
#endif
}

inline void pm_write(FILE* fileHandle, const VnusFloat elt)
{
    fprintf(fileHandle,"%f", elt);
}

inline void pm_write(FILE* fileHandle, const VnusDouble elt)
{
    fprintf(fileHandle,"%f", elt);
}

inline void pm_write(FILE* fileHandle, const VnusComplex elt)
{
#ifdef __GNUC__
    fprintf(fileHandle,"(%f,%f)", std::real( elt ), std::imag( elt ));
#else
    printf("ERROR: pm_write(VnusComplex) not implemented.\n");
    assert(0); exit(1);
#endif
}

inline void pm_write(FILE* fileHandle, VnusBoolean elt)
{
    fprintf(fileHandle,"%s", (int)elt ? "TRUE" : "FALSE");
}

inline void pm_write(FILE* fileHandle, VnusChar elt)
{
    fprintf(fileHandle,"%c", (char)elt);
}

inline void pm_write(FILE* fileHandle, VnusString elt)
{
    fprintf(fileHandle,"%s", elt);
}
#endif
