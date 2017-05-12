// File: f2i.cc
//

#include <vnustypes.h>
#include <vnusstd.h>
#include "spar-rtl.h"

union hack {
    VnusFloat f;
    VnusInt i;
};

VnusInt Java_java_lang_Float_floatToIntBits( VnusFloat v )
{
    hack x;

    x.f = v;
    return x.i;
}
