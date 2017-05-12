// File: i2f.cc
//

#include <vnustypes.h>
#include <vnusstd.h>
#include "spar-rtl.h"

union hack {
    VnusFloat f;
    VnusInt i;
};

VnusFloat Java_java_lang_Float_intBitsToFloat( VnusInt v )
{
    hack x;

    x.i = v;
    return x.f;
}
