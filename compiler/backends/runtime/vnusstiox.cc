/*
 * Open, close and initialize do not use barriers.
 */

#include "vnusstio.h"
#include "io/iopar.h"
#include "io/ioseq.h"
#include "vnusrtl.h"
#include <stdio.h>

VnusInt vp__open(VnusString filename, VnusString access)
{
    VnusInt result = NOT_EXECUTED;
    if (isThisProcessor(0))
    {
        result = vs__open(filename, access);
    }
    return result;
}


VnusInt vp__close(VnusInt fileHandle)
{
    VnusInt result = NOT_EXECUTED;
    if (isThisProcessor(0))
    {
        result = vs__close(fileHandle);
    }
    return result;
}


void vp__initialize(void)
{
    if (isThisProcessor(0))
    {
        vs__initialize();
    }
}

