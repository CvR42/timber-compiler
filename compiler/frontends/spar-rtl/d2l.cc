// File: d2l.cc
//

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <vnustypes.h>
#include <vnusstd.h>
#include "spar-rtl.h"

union hack {
    VnusDouble d;
    VnusLong l;
};

VnusLong Java_java_lang_Double_doubleToLongBits( VnusDouble v )
{
    hack x;

    x.d = v;
    return x.l;
}
