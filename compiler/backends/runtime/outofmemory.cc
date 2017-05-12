// File: outofmemory.cc

#include <stdio.h>
#include <stdlib.h>

#include "outofmemory.h"

void VnusEventOutOfMemory()
{
    fprintf(stderr,"out of memory\n");
    exit(1);
}
