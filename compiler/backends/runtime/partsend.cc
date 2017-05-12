#include "partsend.h"
#include "vnusrtl.h"
#include "shape.h"
#include <vnusstd.h>
#include <vnusprocdefs.h>

//#define TRACE_ON

void vnus_send_partial(VnusInt partialDest, VnusInt depth, VnusInt multiplier, VnusInt replPart, VnusInt numbPart, const int size, const void *buf)
{
    if( depth == processorArrayRank ){
        vnus_send_real(partialDest, size, buf);
    }
    else {
        VnusInt numbPartThis = numbPart%processorArrayDims[depth];
        VnusInt numbPartRest = numbPart/processorArrayDims[depth];
        VnusInt replPartThis = replPart & (VnusInt) 1;
        VnusInt replPartRest = replPart >> 1;
        if( replPartThis ){
            for( VnusInt i=0; i<processorArrayDims[depth]; i++ ){
                vnus_send_partial(
		    partialDest+i*multiplier,
		    depth+1,
		    multiplier*processorArrayDims[depth],
		    replPartRest,
		    numbPartRest,
		    size,
		    buf
		);
	    }
	}
        else {
            vnus_send_partial(
		partialDest+numbPartThis*multiplier,
		depth+1, 
		multiplier*processorArrayDims[depth],
		replPartRest,
		numbPartRest,
		size,
		buf
	    );
	}
    }
}

void vnus_asend_partial(VnusInt partialDest, VnusInt depth, VnusInt multiplier, VnusInt replPart, VnusInt numbPart, const int size, const void *buf)
{
    if( depth == processorArrayRank ){
        vnus_asend_real(partialDest, size, buf);
    }
    else {
        VnusInt numbPartThis = numbPart%processorArrayDims[depth];
        VnusInt numbPartRest = numbPart/processorArrayDims[depth];
        VnusInt replPartThis = replPart & (VnusInt) 1;
        VnusInt replPartRest = replPart >> 1;
        if( replPartThis ){
            for( VnusInt i=0; i<processorArrayDims[depth]; i++ ){
                vnus_asend_partial(
		    partialDest+i*multiplier,
		    depth+1,
		    multiplier*processorArrayDims[depth],
		    replPartRest,
		    numbPartRest,
		    size,
		    buf
		);
	    }
	}
        else {
            vnus_asend_partial(
		partialDest+numbPartThis*multiplier,
		depth+1, 
		multiplier*processorArrayDims[depth],
		replPartRest,
		numbPartRest,
		size,
		buf
	    );
	}
    }
}

/*
  This is a wrapper function that makes use of the vnus_send_partial()
  function to split multiple destinations into different send's.
  This will result in calls to platform specific implementations
  of vnus_send_real().
  */
void vnus_send(const int dest, const int size, const void *buf )
{
#ifdef TRACE_ON
    fprintf(stderr,"v_snd:1\n");
#endif
    VnusInt replicatePart = dest/numberOfProcessors;
    VnusInt numberPart = dest%numberOfProcessors;
#ifdef DEBUG_VnusCommunication
    fprintf(stderr,"vnus_send from %d==(%d,%d) to %d\n",thisProcessor(),
            replicatePart,numberPart,dest);
#endif
#if 0
     fprintf(stderr,"(%d,%d)vs:%d %p %d\n",
            replicatePart,
            numberPart,
            size,
            buf,
            thisProcessor());
#endif
     if (replicatePart == AllProcessorBits())
         vnus_broadcast(size,buf);
     else if (replicatePart != 0) 
         vnus_send_partial(0,0,1,replicatePart,numberPart,size,buf);
     
     else
	 vnus_send_real(numberPart,size,buf);
}

void vnus_asend(const int dest, const int size, const void *buf )
{
#ifdef TRACE_ON
    fprintf(stderr,"v_snd:1\n");
#endif
    VnusInt replicatePart = dest/numberOfProcessors;
    VnusInt numberPart = dest%numberOfProcessors;
#ifdef DEBUG_VnusCommunication
    fprintf(stderr,"vnus_asend from %d==(%d,%d) to %d\n",thisProcessor(),
            replicatePart,numberPart,dest);
#endif
#if 0
     fprintf(stderr,"(%d,%d)vs:%d %p %d\n",
            replicatePart,
            numberPart,
            size,
            buf,
            thisProcessor());
#endif
     if (replicatePart == AllProcessorBits())
         vnus_abroadcast(size,buf);
     else if (replicatePart != 0) 
         vnus_asend_partial(0,0,1,replicatePart,numberPart,size,buf);
     
     else
	 vnus_asend_real(numberPart,size,buf);
}

