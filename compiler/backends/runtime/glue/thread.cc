/* File: vnusthread.cc
 *
 * Service functions for the thread version of the backend.
 */

#include "ptclass.h"

#include <stdio.h>

#include <assert.h>
#include <stdlib.h>

#include "vnusstd.h"
#include "vnusrtl.h"
#include "vnusprocdefs.h"
#include "shape.h"

#include "partsend.h"
#include "vnsupport.h"

/*
 * The User interface
 *
 * This is the only part of this module that is not reentrant due to the
 * use of theRoot.
 */

static VnusThreadBase *theRoot;

void vnus_barrier( void )
{
    theRoot->theBarrier->meet();
}

/* Given a destination processor, the number
 * of elements to send, the size of an element, and a pointer to a
 * buffer, send the elements to the given processor.
 *
 * Any problems are reported within this function, so there is
 * no result.
 */
void vnus_send_real( const int dest, const int size, const void *buf )
{
    theRoot->theThreads[dest]->msgPort[thisProcessor()].Send(size, buf);
}

/* Given a source processor, and the size of the element the number
 * of elements to send, the size of an element, and a pointer to a
 * buffer, send the elements to the given processor.
 *
 * The number of elements that has been received is written in 'nelm',
 * unless the pointer is NULL.
 *
 * Any problems are reported within this function, so there is
 * no result.
 */
void vnus_receive( const int src, const int size, void *buf )
{
#ifdef DEBUG_VnusCommunication
    if (isThisProcessor(0))
        fprintf(stderr,"vnus_recv at %d from %d\n",thisProcessor(),src);
#endif
    theRoot->theThreads[thisProcessor()]->msgPort[src].Recv(size, buf);
}

void vnus_broadcast( const int size, const void *buf )
{
#ifdef DEBUG_VnusCommunication
    if (isThisProcessor(0))
        fprintf(stderr,"vnus_broadcast from %d\n",thisProcessor());
#endif
    for (int dest=0; dest<theRoot->nProc; dest++)
//        if (dest != thisProcessor())
        theRoot->theThreads[dest]->msgPort[thisProcessor()].Send(size, buf);
}

void vnus_exec_forkall( void (*body)( int p, int, char** ), int nproc, int argc, char** argv )
{
    theRoot = new VnusThreadBase(body, nproc, argc, argv);
    theRoot->start();
    delete theRoot;
}

VnusInt thisProcessor()
{
    return theRoot->getThisProcessor();
}

VnusBoolean isThisProcessor(const VnusInt procNo)
{
    return theRoot->isThisProcessor(procNo);
}

void vnus_wait_pending(void)
{}
void vnus_asend_real( const int dest, const int size, const void *buf )
{
    vnus_send_real(dest,size,buf);
}
void vnus_areceive( const int src, const int size, void *buf )
{
    vnus_receive(src,size,buf);
}
void vnus_abroadcast( const int size, const void *buf )
{
    vnus_broadcast(size,buf);
}
