/* File: ptm.cc
 *
 * Service functions for the pentium/trimedia version of the backend.
 */

#include <assert.h>

#include "vnusstd.h"
#include "vnusrtl.h"
#include "vnusprocdefs.h"
#include "partsend.h"

#include <stdio.h>

#include <embedded/embdmalus.h>

#define VERBOSE 0

static VnusInt hiddenThisProcessor;

VnusInt thisProcessor(void)
{
  return hiddenThisProcessor;
}

VnusBoolean isThisProcessor(const VnusInt procNo)
{
  return (hiddenThisProcessor==procNo);
}

void vnus_barrier(void)
{
  emb_barrier();
}

/* Given a destination processor, the number
 * of elements to send, the size of an element, and a pointer to a
 * buffer, send the elements to the given processor.
 *
 * Any problems are reported within this function, so there is
 * no result.
 */

void vnus_send_real(const int dest, const int size, const void *buf )
{
  fprintf(stderr, "WARNING: synchronous send requested (n/a for embedded target).\n");
    emb_send_block(dest, buf, size, 1);
}

void vnus_receive(const int src, const int size, void *buf )
{
  fprintf(stderr, "WARNING: synchronous receive requested (n/a for embedded target).\n");
  if (VERBOSE)
    {
      printf("[%d/%d] enter vnus_receive...\n",emb_PID(),emb_NP());
    }
  emb_recv_block(src,buf,size,1);
}

void vnus_broadcast(const int size, const void *buf )
{
  fprintf(stderr, "WARNING: synchronous broadcast requested (n/a for embedded target).\n");
  for (int i=0; i<numberOfProcessors; i++)
    emb_send_block(i, buf, size, 1);
}

void vnus_wait_pending(void)
{
  emb_fence();
}

void vnus_exec_forkall(void (*body)( int p, int, char** ), const int nproc, const int argc, char** argv )
{
  if (VERBOSE)
    {
      printf("VNUS-forkall\n");
    }

  emb_init(argc, argv, nproc,0);

  hiddenThisProcessor = emb_PID();

  printf("--> proc-id: %d\n",hiddenThisProcessor);

  if (hiddenThisProcessor!=-1)
    {
      printf("[%d] going for body...\n",hiddenThisProcessor);
      fflush(stdout);
      body(hiddenThisProcessor,argc,argv);
      printf("[%d] done body...\n",hiddenThisProcessor);
      fflush(stdout);
    }

  if (VERBOSE)
    {
      printf("emb_done (%d)\n",emb_PID());
    }

  emb_done(1);
}

void vnus_asend_real( const int dest, const int size, const void *buf )
{
  emb_send_block(dest, buf, size, 1);
}
void vnus_areceive( const int src, const int size, void *buf )
{
  emb_recv_block(src, buf, size, 1);
}
void vnus_abroadcast( const int size, const void *buf )
{
  for (int i=0; i<numberOfProcessors; i++)
    emb_send_block(i, buf, size, 1);
}
