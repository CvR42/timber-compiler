// File: notnullassert.cc

#include <stdio.h>
#include <stdlib.h>

#include "notnullassert.h"

void VnusEventNullPointer()
{
    fprintf(stderr,"null pointer detected\n");
    exit(1);
}
