#ifndef __PARTSEND_H__
#define __PARTSEND_H__

#include "vnustypes.h"

void vnus_send_partial(
 VnusInt partialDest,
 VnusInt depth,
 VnusInt multiplier,
 VnusInt replPart,
 VnusInt numbPart,
 const int size,
 const void *buf
);

void vnus_asend_partial(
 VnusInt partialDest,
 VnusInt depth,
 VnusInt multiplier,
 VnusInt replPart,
 VnusInt numbPart,
 const int size,
 const void *buf
);

#endif
