#ifndef __VNUSPROCDEFS_H__
#define __VNUSPROCDEFS_H__

// CvR: quick fix; the endian.h in /usr/local/pthreads/include gets
// upset about the most recent egcs version, since in that one
// the preprocessor variable 'i386' is no longer defined if the
// -ansi flag is given.
#if defined( __i386__ ) && !defined( i386 )
#define i386 __i386__
#endif
#include <shape.h>

// These declarations should occur in all vnus-generated programs.
extern VnusInt numberOfProcessors;
extern VnusInt processorArrayRank;
extern VnusInt processorArrayDims[];

#define AllProcessorBits() (((int)1 << processorArrayRank)-1)
#define AllProcessors()    (AllProcessorBits()*numberOfProcessors)

#endif
