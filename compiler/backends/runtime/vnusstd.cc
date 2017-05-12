// File: vnusstd.cc

#include "vnusstd.h"
#include <stdio.h>
#include <stdlib.h>

void runtime_error( const char *s )
{
    fprintf(stderr,"%s\n",s);
    exit(1);
}

void runtime_warning( const char *s )
{
    fprintf(stderr,"Warning: %s\n", s);
}
