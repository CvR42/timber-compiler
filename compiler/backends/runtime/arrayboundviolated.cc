// File: arrayboundviolated.cc

#include <stdio.h>
#include <stdlib.h>
#include "vnustypes.h"
#include "arrayboundviolated.h"

void VnusEventArrayBoundViolated()
{
    fprintf(stderr,"array bound violated\n");
    exit(1);
}
