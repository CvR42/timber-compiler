/****************/
/** ensemble.c **/
/****************/

unsigned long FENCE_ENTER = 0;
unsigned long FENCE_LEAVE = 0;
unsigned long BARRIER_ENTER = 0;
unsigned long BARRIER_LEAVE = 0;

#define USE_PIO 0
/* #define NDEBUG */

#define VERBOSE 0

#define SHOW_QUEUE 0
#define SHOW_SENDRECV 0
#define SHOW_PRESCHEDULE 0
#define SHOW_STATE_TRANSITIONS 0
#define SHOW_SCS 0
#define CACHE_DISABLE 0
#define SANITY_CHECK 0
#define CHECK_STALLS 0
#define DISABLE_STRIDECPY 0

#define HANDLE_REGISTERS 1
#define DMAMODE dmaAsynchronous

struct CacheLineLong {
  unsigned long value;
  unsigned long dummy[15];
};

typedef struct CacheLineLong CacheLineLong;

static void pio_write_remote(volatile CacheLineLong *addr, unsigned long value);

#include <stdio.h>
#include <assert.h>
#include <tmdrvlib.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#ifdef __GNUC__
#include <sys/wait.h>
#endif

#include "ensemble-support.h"
#define EMB_USE_TIMING
#include "ensemble.h"

#ifdef __TCS__
#include <tm1/mmio.h>
#define RESTRICT restrict
#else
#define RESTRICT
#endif

#define BUFFER_SIZE        4096
#define BUFFERS_PER_QUEUE  3
#define MAX_PROCESSORS     5
#define MAX_QUEUE_ELM      400
#define MAX_DMA_QUEUE      100
#define BPQ                BUFFERS_PER_QUEUE

#define BUFFERS_PER_PROCESSOR (2*BUFFERS_PER_QUEUE*MAX_PROCESSORS)

#define NOPROC   999
#define PENTIUM  101
#define TRIMEDIA 102

#ifdef __TCS__
#include <tmlib/tmlibc.h>
#  define cache_invalidate(buf,len) _cache_invalidate((void *)buf,len)
#  define cache_copyback(buf,len)   _cache_copyback((void *)buf,len)
#endif
#ifdef __GNUC__
#  define cache_invalidate(buf,len)
#  define cache_copyback(buf,len)
#endif

#ifdef __TCS__
#  define cache_malloc(length) _cache_malloc(length,-1)
#  define cache_free(ptr)      _cache_free(ptr)
#endif
#ifdef __GNUC__
#  define cache_malloc(length) malloc(length)
#  define cache_free(ptr)      free(ptr)
#endif

#define MASK_ZEROCOPY 2
#define MASK_REQUEST  1
#define MASK_ADDRESS  (0xfffffffc)

static int emb_procNo,emb_nProcs;
void trace_event(int event);

#include <stdarg.h>

void pprintf (const char *template, ...)
#ifdef __GNUC__
     __attribute__ ((format (printf, 1, 2)))
#endif
;


void pprintf (const char *template, ...)
{
  va_list ap;

  printf("[%d] ",emb_procNo);
  va_start (ap, template);
  vprintf (template, ap);
  va_end (ap);
  fflush(stdout);
}

void fence_info(void)
{
  pprintf("fence-enter: %lu fence-leave: %lu barrier-enter %lu barrier-leave %lu\n",FENCE_ENTER, FENCE_LEAVE,BARRIER_ENTER,BARRIER_LEAVE);
}


/*****************************************************************************
 *                                                                           *
 *                                   DMA Manager                             *
 *                                                                           *
 *****************************************************************************/

struct dma_Summary {
  float N_put;
  float B_put;
  float N_get;
  float B_get;
  unsigned long dummy[12];
};

typedef struct dma_Summary dma_Summary;

static dma_Summary dma_summary;

#ifdef __TCS__

/***************************
 * Trimedia DMA primitives *
 ***************************/

#include <tm1/tmDMA.h>

static Int dma_instance;

struct CompletionVariables {
  volatile CacheLineLong *remote_address;
  unsigned long  remote_value;
  unsigned long *local_address;
  unsigned long  local_value;
};

typedef struct CompletionVariables CompletionVariables;

struct DMA_Queue_Element {
  int in_use;
  CompletionVariables completion;
  dmaRequest_t request;
  int all_the_rest[50];
};

typedef struct DMA_Queue_Element DMA_Queue_Element;

struct DMA_Queue {
  int current;
  volatile int in_use;
  DMA_Queue_Element elm[MAX_DMA_QUEUE];
};

typedef struct DMA_Queue DMA_Queue;

static DMA_Queue dma_queue;

static void dma_completion_function(pdmaRequest_t dma_request)
{
#if USE_PIO
  volatile CacheLineLong *remote_addr;
  unsigned long remote_val;
#endif
  unsigned long *local_addr , local_val;
  
  struct DMA_Queue_Element *dqe;

  /*
  if (dma_request->nr_of_descriptions!=1)
    return;
  */

  dqe = dma_request->data;

  assert(dqe->in_use==1);
  
#if USE_PIO
  remote_addr = dqe->completion.remote_address;
  remote_val  = dqe->completion.remote_value;
#endif
  
  local_addr  = dqe->completion.local_address;
  local_val   = dqe->completion.local_value;
  
#if USE_PIO
  if (remote_addr) pio_write_remote(remote_addr,remote_val);
#endif
  if (local_addr) *local_addr = local_val;
  
  dqe->in_use=0;
  dma_queue.in_use--;
}

static void dma_issue_get(void *dst, void *src, int length,
			  volatile CacheLineLong *remote_addr, unsigned long remote_val,
			  unsigned long *local_addr , unsigned long local_val)
{
  tmLibdevErr_t status;

  assert(dma_queue.elm[dma_queue.current].in_use==0);
  assert(((unsigned long)dst&3)==0);
  assert(((unsigned long)src&3)==0);
  assert((length&3)==0);

  dma_queue.elm[dma_queue.current].in_use = 1;
  dma_queue.in_use++;

#if USE_PIO
  dma_queue.elm[dma_queue.current].request.slack_function = 0;
  dma_queue.elm[dma_queue.current].request.completion_function = dma_completion_function;
  dma_queue.elm[dma_queue.current].request.data = &dma_queue.elm[dma_queue.current];
  dma_queue.elm[dma_queue.current].request.nr_of_descriptions = 1;
  dma_queue.elm[dma_queue.current].request.mode = DMAMODE;
  dma_queue.elm[dma_queue.current].request.priority = 0; /* give priority to PIO-simulating requests */
  dma_queue.elm[dma_queue.current].request.done = 0;
  dma_queue.elm[dma_queue.current].request.descriptions[0].direction            = dmaPCI_TO_SDRAM;
  dma_queue.elm[dma_queue.current].request.descriptions[0].write_and_invalidate = 1;
  dma_queue.elm[dma_queue.current].request.descriptions[0].length               = length;
  dma_queue.elm[dma_queue.current].request.descriptions[0].nr_of_transfers      = 1;
  dma_queue.elm[dma_queue.current].request.descriptions[0].source               = src;
  dma_queue.elm[dma_queue.current].request.descriptions[0].destination          = dst;
  dma_queue.elm[dma_queue.current].request.descriptions[0].source_stride        = 0;
  dma_queue.elm[dma_queue.current].request.descriptions[0].destination_stride   = 0;
  
  dma_queue.elm[dma_queue.current].completion.remote_address = remote_addr;
  dma_queue.elm[dma_queue.current].completion.remote_value   = remote_val;
  dma_queue.elm[dma_queue.current].completion.local_address  = local_addr;
  dma_queue.elm[dma_queue.current].completion.local_value    = local_val;
#else
  if (remote_addr==0)
    {
      dma_queue.elm[dma_queue.current].request.slack_function = 0;
      dma_queue.elm[dma_queue.current].request.completion_function = dma_completion_function;
      dma_queue.elm[dma_queue.current].request.data = &dma_queue.elm[dma_queue.current];
      dma_queue.elm[dma_queue.current].request.nr_of_descriptions = 1;
      dma_queue.elm[dma_queue.current].request.mode = DMAMODE;
      dma_queue.elm[dma_queue.current].request.priority = 0;
      dma_queue.elm[dma_queue.current].request.done = 0;
      dma_queue.elm[dma_queue.current].request.descriptions[0].direction            = dmaPCI_TO_SDRAM;
      dma_queue.elm[dma_queue.current].request.descriptions[0].write_and_invalidate = 1;
      dma_queue.elm[dma_queue.current].request.descriptions[0].length               = length;
      dma_queue.elm[dma_queue.current].request.descriptions[0].nr_of_transfers      = 1;
      dma_queue.elm[dma_queue.current].request.descriptions[0].source               = src;
      dma_queue.elm[dma_queue.current].request.descriptions[0].destination          = dst;
      dma_queue.elm[dma_queue.current].request.descriptions[0].source_stride        = 0;
      dma_queue.elm[dma_queue.current].request.descriptions[0].destination_stride   = 0;
      
      dma_queue.elm[dma_queue.current].completion.remote_address = remote_addr;
      dma_queue.elm[dma_queue.current].completion.remote_value   = remote_val;
      dma_queue.elm[dma_queue.current].completion.local_address  = local_addr;
      dma_queue.elm[dma_queue.current].completion.local_value    = local_val;
    }
  else
    {
      dma_queue.elm[dma_queue.current].request.slack_function = 0;
      dma_queue.elm[dma_queue.current].request.completion_function = dma_completion_function;
      dma_queue.elm[dma_queue.current].request.data = &dma_queue.elm[dma_queue.current];
      dma_queue.elm[dma_queue.current].request.nr_of_descriptions = 2;
      dma_queue.elm[dma_queue.current].request.mode = DMAMODE;
      dma_queue.elm[dma_queue.current].request.priority = 0;
      dma_queue.elm[dma_queue.current].request.done = 0;
      dma_queue.elm[dma_queue.current].request.descriptions[0].direction            = dmaPCI_TO_SDRAM;
      dma_queue.elm[dma_queue.current].request.descriptions[0].write_and_invalidate = 1;
      dma_queue.elm[dma_queue.current].request.descriptions[0].length               = length;
      dma_queue.elm[dma_queue.current].request.descriptions[0].nr_of_transfers      = 1;
      dma_queue.elm[dma_queue.current].request.descriptions[0].source               = src;
      dma_queue.elm[dma_queue.current].request.descriptions[0].destination          = dst;
      dma_queue.elm[dma_queue.current].request.descriptions[0].source_stride        = 0;
      dma_queue.elm[dma_queue.current].request.descriptions[0].destination_stride   = 0;

      dma_queue.elm[dma_queue.current].request.descriptions[1].direction            = dmaSDRAM_TO_PCI;
      dma_queue.elm[dma_queue.current].request.descriptions[1].write_and_invalidate = 1;
      dma_queue.elm[dma_queue.current].request.descriptions[1].length               = 4;
      dma_queue.elm[dma_queue.current].request.descriptions[1].nr_of_transfers      = 1;
      dma_queue.elm[dma_queue.current].request.descriptions[1].source               = &dma_queue.elm[dma_queue.current].completion.remote_value;
      dma_queue.elm[dma_queue.current].request.descriptions[1].destination          = (Pointer)remote_addr;
      dma_queue.elm[dma_queue.current].request.descriptions[1].source_stride        = 0;
      dma_queue.elm[dma_queue.current].request.descriptions[1].destination_stride   = 0;
      
      dma_queue.elm[dma_queue.current].completion.remote_address = 0;
      dma_queue.elm[dma_queue.current].completion.remote_value   = remote_val;
      dma_queue.elm[dma_queue.current].completion.local_address  = local_addr;
      dma_queue.elm[dma_queue.current].completion.local_value    = local_val;

      cache_copyback(&dma_queue.elm[dma_queue.current].completion.remote_value,4);

    }
#endif

  status = dmaDispatch(dma_instance,&dma_queue.elm[dma_queue.current].request);
  assert(status==TMLIBDEV_OK);

  dma_summary.N_get++;
  dma_summary.B_get+= length;

  dma_queue.current= (dma_queue.current+1)%MAX_DMA_QUEUE;
}

static void dma_issue_put(void *dst, void *src, int length,
			  volatile CacheLineLong *remote_addr, unsigned long remote_val,
			  unsigned long *local_addr , unsigned long local_val)
{
  tmLibdevErr_t status;

  assert(dma_queue.elm[dma_queue.current].in_use==0);
  assert(((unsigned long)dst&3)==0);
  assert(((unsigned long)src&3)==0);
  assert((length&3)==0);

  dma_queue.elm[dma_queue.current].in_use = 1;
  dma_queue.in_use++;

#if USE_PIO
  dma_queue.elm[dma_queue.current].request.slack_function = 0;
  dma_queue.elm[dma_queue.current].request.completion_function = dma_completion_function;
  dma_queue.elm[dma_queue.current].request.data = &dma_queue.elm[dma_queue.current];
  dma_queue.elm[dma_queue.current].request.nr_of_descriptions = 1;
  dma_queue.elm[dma_queue.current].request.mode = DMAMODE;
  dma_queue.elm[dma_queue.current].request.priority = 0; /* give priority to PIO-simulating requests */
  dma_queue.elm[dma_queue.current].request.done = 0;
  dma_queue.elm[dma_queue.current].request.descriptions[0].direction            = dmaSDRAM_TO_PCI;
  dma_queue.elm[dma_queue.current].request.descriptions[0].write_and_invalidate = 1;
  dma_queue.elm[dma_queue.current].request.descriptions[0].length               = length;
  dma_queue.elm[dma_queue.current].request.descriptions[0].nr_of_transfers      = 1;
  dma_queue.elm[dma_queue.current].request.descriptions[0].source               = src;
  dma_queue.elm[dma_queue.current].request.descriptions[0].destination          = dst;
  dma_queue.elm[dma_queue.current].request.descriptions[0].source_stride        = 0;
  dma_queue.elm[dma_queue.current].request.descriptions[0].destination_stride   = 0;
  
  dma_queue.elm[dma_queue.current].completion.remote_address = remote_addr;
  dma_queue.elm[dma_queue.current].completion.remote_value   = remote_val;
  dma_queue.elm[dma_queue.current].completion.local_address  = local_addr;
  dma_queue.elm[dma_queue.current].completion.local_value    = local_val;
#else
  if (remote_addr==0)
    {
      dma_queue.elm[dma_queue.current].request.slack_function = 0;
      dma_queue.elm[dma_queue.current].request.completion_function = dma_completion_function;
      dma_queue.elm[dma_queue.current].request.data = &dma_queue.elm[dma_queue.current];
      dma_queue.elm[dma_queue.current].request.nr_of_descriptions = 1;
      dma_queue.elm[dma_queue.current].request.mode = DMAMODE;
      dma_queue.elm[dma_queue.current].request.priority = 0;
      dma_queue.elm[dma_queue.current].request.done = 0;
      dma_queue.elm[dma_queue.current].request.descriptions[0].direction            = dmaSDRAM_TO_PCI;
      dma_queue.elm[dma_queue.current].request.descriptions[0].write_and_invalidate = 1;
      dma_queue.elm[dma_queue.current].request.descriptions[0].length               = length;
      dma_queue.elm[dma_queue.current].request.descriptions[0].nr_of_transfers      = 1;
      dma_queue.elm[dma_queue.current].request.descriptions[0].source               = src;
      dma_queue.elm[dma_queue.current].request.descriptions[0].destination          = dst;
      dma_queue.elm[dma_queue.current].request.descriptions[0].source_stride        = 0;
      dma_queue.elm[dma_queue.current].request.descriptions[0].destination_stride   = 0;
      
      dma_queue.elm[dma_queue.current].completion.remote_address = remote_addr;
      dma_queue.elm[dma_queue.current].completion.remote_value   = remote_val;
      dma_queue.elm[dma_queue.current].completion.local_address  = local_addr;
      dma_queue.elm[dma_queue.current].completion.local_value    = local_val;
    }
  else
    {
      dma_queue.elm[dma_queue.current].request.slack_function = 0;
      dma_queue.elm[dma_queue.current].request.completion_function = dma_completion_function;
      dma_queue.elm[dma_queue.current].request.data = &dma_queue.elm[dma_queue.current];
      dma_queue.elm[dma_queue.current].request.nr_of_descriptions = 2;
      dma_queue.elm[dma_queue.current].request.mode = DMAMODE;
      dma_queue.elm[dma_queue.current].request.priority = 0;
      dma_queue.elm[dma_queue.current].request.done = 0;
      dma_queue.elm[dma_queue.current].request.descriptions[0].direction            = dmaSDRAM_TO_PCI;
      dma_queue.elm[dma_queue.current].request.descriptions[0].write_and_invalidate = 1;
      dma_queue.elm[dma_queue.current].request.descriptions[0].length               = length;
      dma_queue.elm[dma_queue.current].request.descriptions[0].nr_of_transfers      = 1;
      dma_queue.elm[dma_queue.current].request.descriptions[0].source               = src;
      dma_queue.elm[dma_queue.current].request.descriptions[0].destination          = dst;
      dma_queue.elm[dma_queue.current].request.descriptions[0].source_stride        = 0;
      dma_queue.elm[dma_queue.current].request.descriptions[0].destination_stride   = 0;

      dma_queue.elm[dma_queue.current].request.descriptions[1].direction            = dmaSDRAM_TO_PCI;
      dma_queue.elm[dma_queue.current].request.descriptions[1].write_and_invalidate = 1;
      dma_queue.elm[dma_queue.current].request.descriptions[1].length               = 4;
      dma_queue.elm[dma_queue.current].request.descriptions[1].nr_of_transfers      = 1;
      dma_queue.elm[dma_queue.current].request.descriptions[1].source               = &dma_queue.elm[dma_queue.current].completion.remote_value;
      dma_queue.elm[dma_queue.current].request.descriptions[1].destination          = (Pointer)remote_addr;
      dma_queue.elm[dma_queue.current].request.descriptions[1].source_stride        = 0;
      dma_queue.elm[dma_queue.current].request.descriptions[1].destination_stride   = 0;
      
      dma_queue.elm[dma_queue.current].completion.remote_address = 0;
      dma_queue.elm[dma_queue.current].completion.remote_value   = remote_val;
      dma_queue.elm[dma_queue.current].completion.local_address  = local_addr;
      dma_queue.elm[dma_queue.current].completion.local_value    = local_val;

      cache_copyback(&dma_queue.elm[dma_queue.current].completion.remote_value,4);

      /*fprintf(stderr,"DMAPUT SRC:%p, VAL:%d, LENGTH:%d\n",src,*((int *)src),length);/**/

    }
#endif

  status = dmaDispatch(dma_instance,&dma_queue.elm[dma_queue.current].request);
  assert(status==TMLIBDEV_OK);

  dma_summary.N_put++;
  dma_summary.B_put+= length;

  dma_queue.current= (dma_queue.current+1)%MAX_DMA_QUEUE;
}

static void dma_init(void)
{
  int i;

  tmLibdevErr_t status;
  status = dmaOpen(&dma_instance);
  assert(status==TMLIBDEV_OK);

  dma_summary.N_put = 0;
  dma_summary.B_put = 0;
  dma_summary.N_get = 0;
  dma_summary.B_get = 0;

  dma_queue.current = 0;
  dma_queue.in_use  = 0;

  for(i=0;i<MAX_DMA_QUEUE;i++)
    {
      dma_queue.elm[i].in_use = 0;
    }
}

static void dma_done(void)
{
  tmLibdevErr_t status;
  status = dmaClose(dma_instance);
  assert(status==TMLIBDEV_OK);
}

#else

/**************************************************
 * DMA primitives for processors unable to do DMA *
 **************************************************/

/* init and done are null functions, they may be executed;
 * put/get/finished/claim/release, however, indicate a logic error.
 */

static void dma_init(void)
{
  dma_summary.N_put = 0;
  dma_summary.B_put = 0;
  dma_summary.N_get = 0;
  dma_summary.B_get = 0;
}

static void dma_done(void)
{
}

static void dma_issue_put(void *dst, void *src, int length,
			  volatile CacheLineLong *remote_adr, unsigned long remote_val,
			  unsigned long *local_adr, unsigned long local_val)
{
  assert(0);
}

static void dma_issue_get(void *dst, void *src, int length,
			  volatile CacheLineLong *remote_adr, unsigned long remote_val,
			  unsigned long *local_adr, unsigned long local_val)
{
  assert(0);
}

#endif

/*****************************************************************************
 *                                                                           *
 *                             End of: DMA Manager                           *
 *                                                                           *
 *****************************************************************************/

/*****************************************************************************
 *                                                                           *
 *                                Stall Manager                              *
 *                                                                           *
 *****************************************************************************/

#define STALL_MAX 10000
#ifdef __GNUC__
#define STALL_DETECT 50000000
#else
#define STALL_DETECT 500000
#endif
int stall_checkpoint[STALL_MAX];

void stall_init(void)

{
  if (CHECK_STALLS)
    {
      int i;
      for(i=0;i<STALL_MAX;i++)
	stall_checkpoint[i]=0;
    }
}

int stall_detect(int ID)

{
  if (CHECK_STALLS)
    {
      if (++stall_checkpoint[ID]==STALL_DETECT)
	{
	  pprintf("stall detected: ID=%d\n",ID);
	  fence_info();
	  stall_checkpoint[ID]=0;
	  return 1;
	}
    }
  return 0;
}

void stall_progress(int ID)
{
  if (CHECK_STALLS)
    {
      stall_checkpoint[ID]=0;
    }
}

/*****************************************************************************
 *                                                                           *
 *                            End of: Stall Manager                          *
 *                                                                           *
 *****************************************************************************/


/*****************************************************************************
 *                                                                           *
 *                                 PIO manager                               *
 *                                                                           *
 *****************************************************************************/

#ifdef __TCS__

unsigned long pio_read_remote(volatile CacheLineLong *addr)
{
#if USE_PIO
    return addr->value;
#else
    CacheLineLong recv[3];
    tmLibdevErr_t status;
    dmaRequest_t request;

  request.slack_function = 0;
  request.completion_function = 0;
  request.data = 0;
  request.nr_of_descriptions = 1;
  request.mode = dmaSynchronous;
  request.priority = 0;
  request.done = 0;
  request.descriptions[0].direction            = dmaPCI_TO_SDRAM;
  request.descriptions[0].write_and_invalidate = 0;
  request.descriptions[0].length               = 4;
  request.descriptions[0].nr_of_transfers      = 1;
  request.descriptions[0].source               = (Pointer)&addr->value;
  request.descriptions[0].destination          = (Pointer)&recv[1].value;
  request.descriptions[0].source_stride        = 0;
  request.descriptions[0].destination_stride   = 0;
  
  cache_invalidate(&recv[1].value,4);

  status = dmaDispatch(dma_instance,&request);
  assert(status==TMLIBDEV_OK);

  return recv[1].value;
#endif

}

static void pio_write_remote(volatile CacheLineLong *addr, unsigned long value)
{
#if USE_PIO
  addr->value = value;
#else
  tmLibdevErr_t status;

  assert(dma_queue.elm[dma_queue.current].in_use==0);

  dma_queue.elm[dma_queue.current].in_use = 1;
  dma_queue.in_use++;

  dma_queue.elm[dma_queue.current].request.slack_function = 0;
  dma_queue.elm[dma_queue.current].request.completion_function = dma_completion_function;
  dma_queue.elm[dma_queue.current].request.data = &dma_queue.elm[dma_queue.current];
  dma_queue.elm[dma_queue.current].request.nr_of_descriptions = 1;
  dma_queue.elm[dma_queue.current].request.mode = dmaAsynchronous;
  dma_queue.elm[dma_queue.current].request.priority = 0;
  dma_queue.elm[dma_queue.current].request.done = 0;
  dma_queue.elm[dma_queue.current].request.descriptions[0].direction            = dmaSDRAM_TO_PCI;
  dma_queue.elm[dma_queue.current].request.descriptions[0].write_and_invalidate = 1;
  dma_queue.elm[dma_queue.current].request.descriptions[0].length               = 4;
  dma_queue.elm[dma_queue.current].request.descriptions[0].nr_of_transfers      = 1;
  dma_queue.elm[dma_queue.current].request.descriptions[0].source               = &dma_queue.elm[dma_queue.current].completion.remote_value;
  dma_queue.elm[dma_queue.current].request.descriptions[0].destination          = (Pointer)&addr->value;
  dma_queue.elm[dma_queue.current].request.descriptions[0].source_stride        = 0;
  dma_queue.elm[dma_queue.current].request.descriptions[0].destination_stride   = 0;
  
  dma_queue.elm[dma_queue.current].completion.remote_address = 0;
  dma_queue.elm[dma_queue.current].completion.remote_value   = value;
  dma_queue.elm[dma_queue.current].completion.local_address  = 0;
  dma_queue.elm[dma_queue.current].completion.local_value    = 0;

  cache_copyback(&dma_queue.elm[dma_queue.current].completion.remote_value,4);

  status = dmaDispatch(dma_instance,&dma_queue.elm[dma_queue.current].request);
  assert(status==TMLIBDEV_OK);

  dma_queue.current= (dma_queue.current+1)%MAX_DMA_QUEUE;
#endif
}

static unsigned long pio_read_local(volatile CacheLineLong *addr)
{
  cache_invalidate(&addr->value,4);
  return addr->value;
}

static void pio_write_local(volatile CacheLineLong *addr, unsigned long value)
{
  addr->value = value;
  cache_copyback(&addr->value,64);
}

#endif

#ifdef __GNUC__

unsigned long pio_read_remote(volatile CacheLineLong *addr)
{
  return addr->value;
}

static void pio_write_remote(volatile CacheLineLong *addr, unsigned long value)
{
  if (0)
    do
      {
	addr->value = value;
      }
    while (addr->value != value);
  else
    addr->value = value;

}

static unsigned long pio_read_local(volatile CacheLineLong *addr)
{
  return addr->value;
}

static void pio_write_local(volatile CacheLineLong *addr, unsigned long value)
{
  addr->value = value;
}

#endif

/*****************************************************************************
 *                                                                           *
 *                             End of: PIO manager                           *
 *                                                                           *
 *****************************************************************************/


/*****************************************************************************
 *                                                                           *
 *                                 Time Manager                              *
 *                                                                           *
 *****************************************************************************/

/* *** Trimedia high-precision timer *** */

#ifdef __TCS__
#include <tm1/tmTimers.h>

static Int timer_instance;

void timer_init(void)
{
  tmLibdevErr_t result;
  timInstanceSetup_t setup;

  setup.source   = timCLOCK;
  setup.prescale = 1;
  setup.modulus  = 0;
  setup.handler  = 0;
  setup.priority = intPRIO_0;
  setup.running  = 1;

  result = timOpen(&timer_instance);
  assert(result==TMLIBDEV_OK);

  result = timInstanceSetup(timer_instance, &setup);

}

void timer_reset(void)
{
  tmLibdevErr_t result;

  result = timSetTimerValue(timer_instance,0);
  assert(result==TMLIBDEV_OK);
}

void timer_done(void)
{
  tmLibdevErr_t result;

  result = timClose(timer_instance);
  assert(result==TMLIBDEV_OK);
}

unsigned long timer_value(void)
{
  tmLibdevErr_t result;
  unsigned long value;

  result = timGetTimerValue(timer_instance,&value);
  assert(result==TMLIBDEV_OK);

  return value;
}
 

double timer_double(TimerValue DT)
{
  return DT/100000000.0;
}
#endif

/* *** i386 high-precision timer *** */

#ifdef __GNUC__

void timer_init(void)
{
}
void timer_done(void)
{
}
void timer_reset(void)
{
}

__inline__ TimerValue timer_value(void)
{
  unsigned long long int x;
  __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
  return x;
}

double timer_double(TimerValue DT)
{
  return DT/700000000.0;
}

#endif

/*****************************************************************************
 *                                                                           *
 *                             End of: Time Manager                          *
 *                                                                           *
 *****************************************************************************/




/*****************************************************************************
 *                                                                           *
 *                              Stride-Copy Manager                          *
 *                                                                           *
 *****************************************************************************/

#include <string.h>
#include <assert.h>

struct StrideCopyState {
  unsigned char *ptr;
  int ndims;
  int elmSz;
  int finished;
  int expected;
  int remaining;
  int contiguous;
  int i[5];
  int c[5];
  int d[5];
};

static void setup_scs(struct StrideCopyState *scs,
		      const void *ptr, int elmSz, int offset,
		      int ndims, const int *increment, const int *count)
{
  int s[5];
  int c[5];
  int nd;
  int i;
  
  assert(elmSz>0);
  
  scs->ptr      = (unsigned char *)ptr + offset*elmSz;
  scs->finished = 0;
  
  s[0]=1;
  c[0]=elmSz;
  nd=1;
  
  /* copy and collapse as many dimensions as possible. */
  
  for(i=0;i<ndims;i++)
    {
      if (count[i]!=1)
	{
	  int incr = increment[i]*elmSz;
	  
	  if (incr==c[nd-1]*s[nd-1]) /* dimension can be collapsed */
	    {
	      c[nd-1]*=count[i];
	    }
	  else
	    { /* dimension adds information. */
	      assert(nd<5);
	      s[nd] = incr;
	      c[nd] = count[i];
	      nd++;
	    }
	}
    }
  
  /* check whether the buffer is contiguous */

  scs->contiguous = (nd==1);

  /* c[0] contains "basic element" size, and has a guaranteed stride of 1.
   *
   * We must now normalize this such that (c[0],s[0]) contains a practical
   * form for packing as these will become the inner loop iterators.
   *
   * Lowering the element size, while at the same time enlarging the count
   * will in effect introduce an extra dimension. This happens if after this
   * c[0]!=1.
   *
   * We PREFER that elmSize is 4 (special code is in place for that);
   * We must GUARANTEE that elmSize<BUF_SIZE. In the latter case, it should be
   * the highest possible value, meaning that count should have the lowest
   * possible value.
   *
   * Note that this code swaps the sense of s[0] and c[0]!
   *
   */

  if ((c[0]&3)==0)
    {
      /* the inner-loop element-size will be four, which is good. */
      
      s[0]=4;
      c[0]>>=2;
    }
  else
    {
      s[0]=c[0];
      c[0]=1;
      
      assert(c[0]<BUFFER_SIZE);
      
      /*
	for(s[0]=1;;s[0]++)
	if (c[0]%s[0]==0 && (c[0]/s[0])<=BUFFER_SIZE)
	break;
	
	c[0]/=s[0];
      */      
    }

  /* s[0] is now the element size, while c[0] is the inner-loop count */

  if (SHOW_SCS)
    {
      int i;
      pprintf("after collapse (nd==%d):\n",nd);
      for(i=0;i<nd;i++)
	pprintf(" s[%d] = %10d   c[%d] = %10d\n",i,s[i],i,c[i]);
    }

  if (c[0]==1) /* the first dimension is really just an element size! */
    {
      if (nd==1)
	{
	  scs->ndims = 0;
	  scs->elmSz = s[0];
	  scs->expected = s[0];
	  scs->remaining = scs->expected;
	  /*fprintf(stderr,"YEEHAH:%d\n",scs->remaining);/**/
	  return;
	}

      scs->ndims = nd-1;
      scs->elmSz = s[0];
      scs->d[0]  = s[1];

      scs->expected = scs->elmSz * (scs->i[0] = scs->c[0] = c[1]);

      for(i=(scs->ndims)-1;i>0;i--)
	{
	  scs->d[i] = (s[i+1]-c[i]*s[i]);
	  scs->expected*=(scs->i[i] = scs->c[i] = c[i+1]);
	}
    }
  else /* inner loop is really a separate dimension! */
    {
      scs->ndims = nd;
      scs->elmSz = s[0];
      scs->d[0]  = s[0];

      scs->expected = scs->elmSz* (scs->i[0]  = scs->c[0] = c[0]);

      for(i=(scs->ndims)-1;i>0;i--)
	{
	  scs->d[i] = (s[i]-c[i-1]*s[i-1]);
	  scs->expected*=(scs->i[i] = scs->c[i] = c[i]);
	}
    }

  scs->remaining = scs->expected;
  /*fprintf(stderr,"scs-remaining:%d\n",scs->remaining);/**/

  if (SHOW_SCS)
    {
      int i;
      pprintf("at end: scs = %p ndims=%d elmSz=%d expected=%d contiguous=%d\n",
	     scs,scs->ndims,scs->elmSz,scs->expected,scs->contiguous);

      for(i=0;i<scs->ndims;i++)
	pprintf(" i[%d] = %10d  c[%d] = %10d d[%d]=%10d\n",
	       i,scs->i[i],i,scs->c[i],i,scs->d[i]);
    }

}

int do_memcpy_stride_to_linear(struct StrideCopyState *scs, void *dst, unsigned int maxBytes)
{
  int elmSz = scs->elmSz;
  int ndims=scs->ndims;
  int procElm,maxElm;

  if (ndims==0)
    {
      memcpy(dst,scs->ptr,elmSz);
      scs->remaining = 0;
      scs->finished = 1;
      return elmSz; /* all done */
    }

  procElm=0;
  
  if (elmSz==4) /* important case! */
    {
      const int d0_1 = scs->d[0];
      const int d0_2 = d0_1+d0_1;
      const int d0_3 = d0_2+d0_1;
      const int d0_4 = d0_3+d0_1;
      const int d0_5 = d0_4+d0_1;
      const int d0_6 = d0_5+d0_1;
      const int d0_7 = d0_6+d0_1;
      const int d0_8 = d0_7+d0_1;
      
      unsigned char * ptr0 = scs->ptr;
      unsigned long * dst0 = (unsigned long *)dst;
      
      maxElm = (maxBytes>>2);
      while(1)
	{
	  int z;
	  int count;
	  
	  if (scs->i[0]<maxElm-procElm)
	    {
	      count = scs->i[0]; /* stop at dimension end */
	      procElm += count;
	      scs->i[0]=0;
	    }
	  else
	    {
	      count = maxElm-procElm;
	      procElm=maxElm; /* stop at buffer fill */
	      scs->i[0]-=count;
	    }

	  if (DISABLE_STRIDECPY)
	    count = 0;

	  while((count&7)!=0) {
	    *dst0++ = *(unsigned long *)ptr0;
	    ptr0 += d0_1;
	    --count;
	  }

	  if (count)
	    {
	      unsigned long v0,v1,v2,v3,v4,v5,v6,v7;

	      unsigned char * RESTRICT ptr1 = ptr0+d0_1;
	      unsigned char * RESTRICT ptr2 = ptr0+d0_2;
	      unsigned char * RESTRICT ptr3 = ptr0+d0_3;
	      unsigned char * RESTRICT ptr4 = ptr0+d0_4;
	      unsigned char * RESTRICT ptr5 = ptr0+d0_5;
	      unsigned char * RESTRICT ptr6 = ptr0+d0_6;
	      unsigned char * RESTRICT ptr7 = ptr0+d0_7;
	      
	      unsigned long * RESTRICT dst1 = dst0+1;
	      unsigned long * RESTRICT dst2 = dst0+2;
	      unsigned long * RESTRICT dst3 = dst0+3;
	      unsigned long * RESTRICT dst4 = dst0+4;
	      unsigned long * RESTRICT dst5 = dst0+5;
	      unsigned long * RESTRICT dst6 = dst0+6;
	      unsigned long * RESTRICT dst7 = dst0+7;
	      
	      do
		{
		  v0 = *(unsigned long *)ptr0; ptr0+=d0_8;
		  v1 = *(unsigned long *)ptr1; ptr1+=d0_8;
		  v2 = *(unsigned long *)ptr2; ptr2+=d0_8;
		  v3 = *(unsigned long *)ptr3; ptr3+=d0_8;
		  v4 = *(unsigned long *)ptr4; ptr4+=d0_8;
		  v5 = *(unsigned long *)ptr5; ptr5+=d0_8;
		  v6 = *(unsigned long *)ptr6; ptr6+=d0_8;
		  v7 = *(unsigned long *)ptr7; ptr7+=d0_8;
		  
		  *dst0 = v0; dst0+=8;
		  *dst1 = v1; dst1+=8;
		  *dst2 = v2; dst2+=8;
		  *dst3 = v3; dst3+=8;
		  *dst4 = v4; dst4+=8;
		  *dst5 = v5; dst5+=8;
		  *dst6 = v6; dst6+=8;
		  *dst7 = v7; dst7+=8;
		  
		} while(count-=8);
	      
	  }

	  if (procElm>=maxElm)
	    {
	      scs->ptr       = ptr0;
	      scs->remaining-=(procElm<<2);
	      scs->finished  = (scs->remaining==0);
	      return (procElm<<2); /* not done yet! */
	    }
	  
	  z=0;
	  do {
	    scs->i[z] = scs->c[z];
	    if (ndims==++z)
	      {
		/* all done */
		scs->ptr       = ptr0;
		scs->remaining-=(procElm<<2);
		scs->finished  = (scs->remaining==0);
		return (procElm<<2);
	      }
	    ptr0 += scs->d[z];
	  } while(--(scs->i[z])==0);
	}
    }
  
  else /* element size != 4 */
  
    {
      unsigned char *the_ptr = scs->ptr;
      unsigned char *the_dst = dst;
	  
      const int d0 = scs->d[0];

      int maxElm = maxBytes/elmSz;      
      while(1)
	{
	  int i0,z,count;

	  i0 = scs->i[0];
	  
	  count = maxElm-procElm;
	  if (i0<count) count=i0;

	  procElm+=count;
	  i0-=count;

	  while(count--)
	    {
	      memcpy(the_dst, the_ptr, elmSz);
	      the_dst+=elmSz;
	      the_ptr+=d0;
	    }
	  
	  if (procElm>=maxElm)
	    {
	      scs->i[0]      = i0;
	      scs->ptr       = the_ptr;
	      scs->remaining-=(procElm*elmSz);
	      scs->finished  = (scs->remaining==0);
	      return procElm*elmSz; /* not done yet! */
	    }
	  
	  z=0;
	  do {
	    scs->i[z] = scs->c[z];
	    if (ndims==++z)
	      {
		/* all done */
		scs->ptr       = the_ptr;
		scs->remaining-= (procElm*elmSz);
		scs->finished  = (scs->remaining==0);
		return procElm*elmSz;
	      }
	    the_ptr += scs->d[z];
	  } while(--(scs->i[z])==0);
	}
    }
}

int do_memcpy_linear_to_stride(struct StrideCopyState *scs, void *src, unsigned int maxBytes)
{
  int elmSz     = scs->elmSz;
  int ndims=scs->ndims;
  int procElm, maxElm;

  if (ndims==0)
    {
      memcpy(scs->ptr,src,elmSz);

      scs->remaining = 0;
      scs->finished = 1;
      return elmSz; /* all done */
    }

  procElm=0;

  if (elmSz==4) /* important case! */
    {
      const int d0_1 = scs->d[0];
      const int d0_2 = d0_1+d0_1;
      const int d0_3 = d0_2+d0_1;
      const int d0_4 = d0_3+d0_1;
      const int d0_5 = d0_4+d0_1;
      const int d0_6 = d0_5+d0_1;
      const int d0_7 = d0_6+d0_1;
      const int d0_8 = d0_7+d0_1;
      
      int procElm   = 0;
      
      unsigned char * ptr0 = scs->ptr;
      unsigned long * src0 = (unsigned long *)src;

      maxElm = (maxBytes>>2);
      while(1)
	{
	  int z;
	  int count;

	  if (scs->i[0]<maxElm-procElm)
	    {
	      count = scs->i[0]; /* stop at dimension end */
	      procElm += count;
	      scs->i[0]=0;
	    }
	  else
	    {
	      count = maxElm-procElm;
	      procElm=maxElm; /* stop at buffer fill */
	      scs->i[0]-=count;
	    }

	  if (DISABLE_STRIDECPY)
	    count = 0;

	  while((count&7)!=0) {
	    *(unsigned long *)ptr0 = *src0++;
	    ptr0 += d0_1;
	    --count;
	  }
	  
	  if (count)
	    {
	      unsigned long v0,v1,v2,v3,v4,v5,v6,v7;

	      unsigned char * RESTRICT ptr1 = ptr0+d0_1;
	      unsigned char * RESTRICT ptr2 = ptr0+d0_2;
	      unsigned char * RESTRICT ptr3 = ptr0+d0_3;
	      unsigned char * RESTRICT ptr4 = ptr0+d0_4;
	      unsigned char * RESTRICT ptr5 = ptr0+d0_5;
	      unsigned char * RESTRICT ptr6 = ptr0+d0_6;
	      unsigned char * RESTRICT ptr7 = ptr0+d0_7;
	      
	      unsigned long * RESTRICT src1 = src0+1;
	      unsigned long * RESTRICT src2 = src0+2;
	      unsigned long * RESTRICT src3 = src0+3;
	      unsigned long * RESTRICT src4 = src0+4;
	      unsigned long * RESTRICT src5 = src0+5;
	      unsigned long * RESTRICT src6 = src0+6;
	      unsigned long * RESTRICT src7 = src0+7;
	      
	      do {
		v0 = *src0; src0+=8;
		v1 = *src1; src1+=8;
		v2 = *src2; src2+=8;
		v3 = *src3; src3+=8;
		v4 = *src4; src4+=8;
		v5 = *src5; src5+=8;
		v6 = *src6; src6+=8;
		v7 = *src7; src7+=8;
		
		*(unsigned long *)ptr0 = v0; ptr0+=d0_8;
		*(unsigned long *)ptr1 = v1; ptr1+=d0_8;
		*(unsigned long *)ptr2 = v2; ptr2+=d0_8;
		*(unsigned long *)ptr3 = v3; ptr3+=d0_8;
		*(unsigned long *)ptr4 = v4; ptr4+=d0_8;
		*(unsigned long *)ptr5 = v5; ptr5+=d0_8;
		*(unsigned long *)ptr6 = v6; ptr6+=d0_8;
		*(unsigned long *)ptr7 = v7; ptr7+=d0_8;
		
	      } while(count-=8);
	    }
	  
	  if (procElm>=maxElm)
	    {
	      scs->ptr       = ptr0;
	      scs->remaining-=(procElm<<2);
	      scs->finished  = (scs->remaining==0);
	      return (procElm<<2); /* not done yet! */
	    }
	  
	  z=0;
	  do {
	    scs->i[z] = scs->c[z];
	    if (ndims==++z)
	      {
		/* all done */
		scs->ptr       = ptr0;
		scs->remaining-=(procElm<<2);
		scs->finished  = (scs->remaining==0);
		return (procElm<<2);
	      }
	    ptr0 += scs->d[z];
	  } while(--(scs->i[z])==0);
	}
    }
  
  else /* element size != 4 */

    {
      unsigned char *the_ptr = scs->ptr;
      unsigned char *the_src = src;

      const int d0 = scs->d[0];

      int maxElm = maxBytes/elmSz;
      while(1)
	{
	  int i0,z,count;
	  
	  i0 = scs->i[0];
	  
	  count = maxElm-procElm;
	  if (i0<count) count=i0;

	  procElm+=count;
	  i0-=count;

	  while(count--)
	    {
	      memcpy(the_ptr, the_src, elmSz);
	      the_ptr += d0;
	      the_src += elmSz;
	    }

	  if (procElm>=maxElm)
	    {
	      scs->i[0]      = i0;
	      scs->ptr       = the_ptr;
	      scs->remaining-= (procElm*elmSz);
	      scs->finished  = (scs->remaining==0);
	      return procElm*elmSz; /* not done yet! */
	    }
	  
	  z=0;
	  do {
	    scs->i[z] = scs->c[z];
	    if (ndims==++z)
	      {
		/* all done */
		scs->ptr       = the_ptr;
		scs->finished  = 1;
		scs->remaining = 0;
		return procElm*elmSz;
	      }
	    the_ptr += scs->d[z];
	  } while(--(scs->i[z])==0);
	}
    }
}

/*****************************************************************************
 *                                                                           *
 *                            End of: Stride-Copy Manager                    *
 *                                                                           *
 *****************************************************************************/

struct PersonalPage {
  volatile CacheLineLong BARRIER[MAX_PROCESSORS];
  volatile CacheLineLong INCOMING_REQUEST[MAX_PROCESSORS];
  volatile CacheLineLong OUTGOING_CONFIRM[MAX_PROCESSORS];
  volatile CacheLineLong BARCOUNT;
  volatile CacheLineLong PROCTYPE;
};

typedef struct PersonalPage PersonalPage;

struct BufferPage {
  unsigned char data[BUFFER_SIZE];
};

typedef struct BufferPage BufferPage;

struct BootPage {
  volatile unsigned long NPROCS;
  volatile unsigned long READY[MAX_PROCESSORS];
  volatile unsigned long PAGES[MAX_PROCESSORS][1+BUFFERS_PER_PROCESSOR];
};

typedef struct BootPage BootPage;

static int emb_procNo = -1;
static int emb_nProcs = -1;

static clock_t emb_init_time;
static clock_t emb_done_time;

static volatile BootPage *emb_bootPage = 0;

static volatile PersonalPage *emb_personal[MAX_PROCESSORS];
static unsigned long          emb_procType[MAX_PROCESSORS];

static volatile PersonalPage *emb_myPersonal;

static BufferPage *emb_buffers[MAX_PROCESSORS][BUFFERS_PER_PROCESSOR];
static BufferPage *emb_myBuffer[BUFFERS_PER_PROCESSOR];

/*****************************************************************************
 *                                                                           *
 *                                Trace Manager                              *
 *                                                                           *
 *****************************************************************************/

struct TraceEvent {
  int event;
  TimerValue T;
};

struct TraceEvent TRACE[1000];
int NTRACE=0;

void trace_event(int event)
{
  if (NTRACE<1000)
    {
      TRACE[NTRACE].event = event;
      TRACE[NTRACE].T = timer_value();
      ++NTRACE;
    }
}

void trace_print(void)
{
  int i;
  for(i=0;i<NTRACE;i++)
    {
      pprintf("%5d event %5d time %10lu\n",i,TRACE[i].event, (unsigned long)(TRACE[i].T-TRACE[0].T));
    }
}

/*****************************************************************************
 *                                                                           *
 *                          End of: Trace Manager                            *
 *                                                                           *
 *****************************************************************************/

int emb_NP(void)
{
  return emb_nProcs;
}

int emb_PID(void)
{
  return emb_procNo;
}

#ifdef __GNUC__
static char *BASEPTR[MAX_PROCESSORS];

static void *map2user(int procNo, void *pointer)
{
  unsigned long OFFSET = ((unsigned long)pointer)&0x7fffff;
  char *NEWPTR;
  
  NEWPTR = &BASEPTR[procNo][OFFSET];
  
  return NEWPTR;
}
#endif

void emb_barrier2(void)
     /* this does NOT work if processor 0 is not the first to enter! */
{
  ++BARRIER_ENTER;
  if (emb_procNo==0) /* barrier-host */
    {
      int i;
      for(i=1;i<emb_nProcs;i++)
        {
	  unsigned NBAR=0;
	  do {
	    if (CHECK_STALLS)
	    if (++NBAR==STALL_DETECT)
	      {
		pprintf("barrier-stall #1\n");
		fence_info();
		NBAR=0;
	      }
	  } while (pio_read_local(&emb_myPersonal->BARRIER[i])==0);
	  
	  pio_write_local(&emb_myPersonal->BARRIER[i],0);
        }
      
      for(i=1;i<emb_nProcs;i++)
	{
	  pio_write_remote(&emb_personal[i]->BARRIER[0],1);
	}
    }
  else /* no-barrier-host */
    {
      unsigned NBAR=0;
      pio_write_remote(&emb_personal[0]->BARRIER[emb_procNo],1);
      do {
	if (CHECK_STALLS)
	if (++NBAR==STALL_DETECT)
	  {
	    pprintf("barrier-stall #2\n");
	    NBAR=0;
	  }
      } while (pio_read_local(&emb_myPersonal->BARRIER[0])==0);

      pio_write_local(&emb_myPersonal->BARRIER[0],0);
    }
  ++BARRIER_LEAVE;
}

void emb_barrier(void)
{
  ++BARRIER_ENTER;
  if (emb_procNo==0) /* barrier-host */
    {
      int i;
      for(i=1;i<emb_nProcs;i++)
        {
	  unsigned NBAR=0;
	  do {
	    if (CHECK_STALLS)
	      if (++NBAR==STALL_DETECT)
		{
		  pprintf("barrier-stall #1\n");
		  fence_info();
		  NBAR=0;
		}
	  } while (pio_read_local(&emb_myPersonal->BARRIER[i])==0);
        }
      
      for(i=1;i<emb_nProcs;i++)
	{
	  pio_write_remote(&emb_personal[i]->BARRIER[0],1);
	}

      for(i=1;i<emb_nProcs;i++)
	do {
	  unsigned NBAR=0;
	  if (CHECK_STALLS)
	    if (++NBAR==STALL_DETECT)
	      {
		pprintf("barrier-stall #2\n");
		fence_info();
		NBAR=0;
	      }
	} while (pio_read_local(&emb_myPersonal->BARRIER[i])!=0);
      
      for(i=1;i<emb_nProcs;i++)
	{
	  pio_write_remote(&emb_personal[i]->BARRIER[0],0);
	}

    }
  else /* no-barrier-host */
    {
      unsigned NBAR=0;

      pio_write_remote(&emb_personal[0]->BARRIER[emb_procNo],1);
      do {
	if (CHECK_STALLS)
	  if (++NBAR==STALL_DETECT)
	    {
	      pprintf("barrier-stall #3\n");
	      fence_info();
	      NBAR=0;
	    }
      } while (pio_read_local(&emb_myPersonal->BARRIER[0])==0);

      pio_write_remote(&emb_personal[0]->BARRIER[emb_procNo],0);

      do {
	unsigned NBAR=0;
	if (CHECK_STALLS)
	  if (++NBAR==STALL_DETECT)
	    {
	      pprintf("barrier-stall #4\n");
	      fence_info();
	      NBAR=0;
	    }
      } while (pio_read_local(&emb_myPersonal->BARRIER[0])!=0);

    }
  ++BARRIER_LEAVE;
}

void emb_pbarrier(void)
{
  if (emb_procNo>=2) return;

  ++BARRIER_ENTER;
  if (emb_procNo==0) /* barrier-host */
    {
      int i;
      for(i=1;i<2;i++)
        {
	  unsigned NBAR=0;
	  do {
	    if (CHECK_STALLS)
	      if (++NBAR==STALL_DETECT)
		{
		  pprintf("barrier-stall #1\n");
		  fence_info();
		  NBAR=0;
		}
	  } while (pio_read_local(&emb_myPersonal->BARRIER[i])==0);
        }
      
      for(i=1;i<2;i++)
	{
	  pio_write_remote(&emb_personal[i]->BARRIER[0],1);
	}

      for(i=1;i<2;i++)
	do {
	  unsigned NBAR=0;
	  if (CHECK_STALLS)
	    if (++NBAR==STALL_DETECT)
	      {
		pprintf("barrier-stall #2\n");
		fence_info();
		NBAR=0;
	      }
	} while (pio_read_local(&emb_myPersonal->BARRIER[i])!=0);
      
      for(i=1;i<2;i++)
	{
	  pio_write_remote(&emb_personal[i]->BARRIER[0],0);
	}

    }
  else /* no-barrier-host */
    {
      unsigned NBAR=0;
      pio_write_remote(&emb_personal[0]->BARRIER[emb_procNo],1);
      do {
	if (CHECK_STALLS)
	  if (++NBAR==STALL_DETECT)
	    {
	      pprintf("barrier-stall #3\n");
	      fence_info();
	      NBAR=0;
	    }
      } while (pio_read_local(&emb_myPersonal->BARRIER[0])==0);

      pio_write_remote(&emb_personal[0]->BARRIER[emb_procNo],0);

      do {
	unsigned NBAR=0;
	if (CHECK_STALLS)
	  if (++NBAR==STALL_DETECT)
	    {
	      pprintf("barrier-stall #4\n");
	      fence_info();
	      NBAR=0;
	    }
      } while (pio_read_local(&emb_myPersonal->BARRIER[0])!=0);

    }
  ++BARRIER_LEAVE;
}

/*****************************************************************************
 *                                                                           *
 *                          Send/Receive/Fence Stuff                         *
 *                                                                           *
 *****************************************************************************/

struct StrideQueueElm {
  unsigned long flags;
  unsigned long nDmaCompleted,nUnpacked,nPacked,nConfirmed,nDmaIssued;
  unsigned char *address;
  struct StrideCopyState scs;
};

struct StrideQueue {
  int nElm;
  int curElm;
  int state;
  unsigned long n_packFrags;
  unsigned long n_dmaFrags;
  BufferPage *LOCAL[BUFFERS_PER_QUEUE];
  BufferPage *REMOTE[BUFFERS_PER_QUEUE];
  int LOCAL_BYTES[BUFFERS_PER_QUEUE];
  int REMOTE_BYTES[BUFFERS_PER_QUEUE];
  struct StrideQueueElm elm[MAX_QUEUE_ELM];
};

struct StrideQueue incoming[MAX_PROCESSORS];
struct StrideQueue outgoing[MAX_PROCESSORS];

/*****************************************************************************
 *                                                                           *
 *                                 emb_send()                                *
 *                                                                           *
 *****************************************************************************/

void emb_send(int dstProc, int elmSize, const void *base, int offset,
	      int num_strides, const int *strides, const int *stride_count,
	      int flags)
{
  if (SHOW_SENDRECV)
    {
      int i;
      pprintf("emb_send: dstProc     = %d\n",dstProc);
      pprintf("          elmSize     = %d\n",elmSize);
      pprintf("          base        = %p\n",base);
      pprintf("          offset      = %d\n",offset);
      pprintf("          num_strides = %d\n",num_strides);
      for(i=0;i<num_strides;i++)
	pprintf("            stride[%d] = %4d, count[%d] = %4d\n",
	       i,strides[i],i,stride_count[i]);
      pprintf("          flags       = %d\n",flags);
      pprintf("          dma queue # = %d\n",
	     outgoing[dstProc].nElm);
    }

  assert(outgoing[dstProc].nElm<MAX_QUEUE_ELM);

  /*fprintf(stderr,"emb_send:BASE:%p\n",base);/**/
  setup_scs(&outgoing[dstProc].elm[outgoing[dstProc].nElm].scs,
	    base,elmSize,offset,num_strides,strides,stride_count);

  if (HANDLE_REGISTERS)
    if (outgoing[dstProc].elm[outgoing[dstProc].nElm].scs.expected!=0)
      {
	outgoing[dstProc].elm[outgoing[dstProc].nElm].flags = flags;
	outgoing[dstProc].nElm++;
      }
}

void emb_send_block(int dstProc, const void *base, int size, int flags)
{
  emb_send(dstProc, size, base, 0, 0, 0, 0, flags);
}

/*****************************************************************************
 *                                                                           *
 *                                 emb_recv()                                *
 *                                                                           *
 *****************************************************************************/

void emb_recv(int srcProc, int elmSize, const void *base, int offset,
	      int num_strides, const int *strides, const int *stride_count,
	      int flags)
{
  if (SHOW_SENDRECV)
    {
      int i;
      pprintf("emb_recv: srcProc     = %d\n",srcProc);
      pprintf("          elmSize     = %d\n",elmSize);
      pprintf("          base        = %p\n",base);
      pprintf("          offset      = %d\n",offset);
      pprintf("          num_strides = %d\n",num_strides);
      for(i=0;i<num_strides;i++)
	pprintf("            stride[%d] = %4d, count[%d] = %4d\n",
	       i,strides[i],i,stride_count[i]);
      pprintf("[          flags       = %d\n",flags);
      pprintf("          dma queue # = %d\n",
	     incoming[srcProc].nElm);
    }

  assert(incoming[srcProc].nElm<MAX_QUEUE_ELM);

  setup_scs(&incoming[srcProc].elm[incoming[srcProc].nElm].scs,
	    base,elmSize,offset,num_strides,strides,stride_count);


  if (HANDLE_REGISTERS)
    if (incoming[srcProc].elm[incoming[srcProc].nElm].scs.expected!=0)
      {
	incoming[srcProc].elm[incoming[srcProc].nElm].flags = flags;
	incoming[srcProc].nElm++;
      }
}

void emb_recv_block(int srcProc, const void *base, int size, int flags)
{
  emb_recv(srcProc, size, base, 0, 0, 0, 0, flags);
}

/*****************************************************************************
 *                                                                           *
 *                              emb_process_outgoing()                       *
 *                                                                           *
 *****************************************************************************/

static int emb_process_outgoing(int peer, int state, int curElm)
{
  if (SHOW_QUEUE)
    pprintf("emb_process_outgoing: peer = %d state = %d\n",peer,state);

  switch(state)
    {
    case 0: /* INITIAL STATE */
      if (emb_procNo==peer) /* send-to-self? */
	{
	  outgoing[peer].elm[curElm].nDmaCompleted = 0;
	  outgoing[peer].elm[curElm].nPacked       = 0;
	  outgoing[peer].elm[curElm].nUnpacked     = 0;
	  outgoing[peer].elm[curElm].nConfirmed    = 0;
	  outgoing[peer].elm[curElm].nDmaIssued    = 0;
	  goto case_100;
	}
      switch(emb_procType[emb_procNo])
	{
	case TRIMEDIA:
	  switch(emb_procType[peer])
	    {
	    case TRIMEDIA:
	      outgoing[peer].elm[curElm].nDmaCompleted = 0;
	      outgoing[peer].elm[curElm].nPacked       = 0;
	      outgoing[peer].elm[curElm].nUnpacked     = 0;
	      outgoing[peer].elm[curElm].nConfirmed    = 0;
	      outgoing[peer].elm[curElm].nDmaIssued    = 0;
	      goto case_200; /* TRIMEDIA -> TRIMEDIA */
	    case PENTIUM:
	      outgoing[peer].elm[curElm].nDmaCompleted = 0;
	      outgoing[peer].elm[curElm].nPacked       = 0;
	      outgoing[peer].elm[curElm].nUnpacked     = 0;
	      outgoing[peer].elm[curElm].nConfirmed    = 0;
	      outgoing[peer].elm[curElm].nDmaIssued    = 0;
	      goto case_200; /* TRIMEDIA -> HOST */
	    }
	  break;
	case PENTIUM:
	  switch(emb_procType[peer])
	    {
	    case TRIMEDIA:
	      outgoing[peer].elm[curElm].nDmaCompleted = 0;
	      outgoing[peer].elm[curElm].nPacked       = 0;
	      outgoing[peer].elm[curElm].nUnpacked     = 0;
	      outgoing[peer].elm[curElm].nConfirmed    = 0;
	      outgoing[peer].elm[curElm].nDmaIssued    = 0;
	      goto case_300; /* HOST -> TRIMEDIA */
	    default:
	      assert(0);
	    }
	  break;
	default:
	  assert(0);
	}
      
      /********************************
       * states 100-101: send-to-self *
       ********************************/
      
    case_100:
    case 100:
      /* pack strided-to-linear the outgoing buffer to LOCAL[0]. */
      outgoing[peer].LOCAL_BYTES[0] =
	do_memcpy_stride_to_linear(&outgoing[peer].elm[curElm].scs,
				   outgoing[peer].LOCAL[0],BUFFER_SIZE);
      return 101;

    case 101:
      /* unpack linear-to-stride LOCAL[0], using "incoming" specification. */
      outgoing[peer].LOCAL_BYTES[0] =
	do_memcpy_linear_to_stride(&incoming[peer].elm[curElm].scs,
				   outgoing[peer].LOCAL[0],BUFFER_SIZE);
      /* next-state: 0 if finished, 101 if not-finished. */
      return incoming[peer].elm[curElm].scs.finished ? 0 : 100;
      
      /***************************
       * states 2xx: send TM->TM *
       ***************************/
      
    case_200:
    case 200:
      
      /* we must wait for a receive-request by the remote Trimedia, to see
       * if it can handle a non-buffered receive.
       */
      
      {
	unsigned long REQUEST,KIND;

	/* check whether the receiver has posted a REQUEST. */

	REQUEST = pio_read_local(&emb_myPersonal->INCOMING_REQUEST[peer]);
	
	if (stall_detect(3999))
	  {
	    pprintf("outgoing-200 peer=%d curElm=%d REQUEST=%lu\n",
		   peer,curElm,REQUEST);
	  }

	if (REQUEST==0)
	  return 200; /* no request yet. try again later. */

	stall_progress(3999);

	/*
	 * Determine the kind of transfer:
	 *
	 *  KIND     receiver         sender
	 *  ----    ----------      ----------
	 *     0     buffered        buffered
	 *     1     buffered       unbuffered
	 *     2    unbuffered       buffered
	 *     3    unbuffered      unbuffered
	 */

	KIND = (REQUEST&MASK_ZEROCOPY) | outgoing[peer].elm[curElm].scs.contiguous;

	switch(KIND)
	  {
	  case 0: /* buffered   -> buffered */
	    outgoing[peer].elm[curElm].nDmaIssued=0;
	    goto case_202;
	  case 1: /* unbuffered -> buffered */
	    cache_copyback(outgoing[peer].elm[curElm].scs.ptr,
			   outgoing[peer].elm[curElm].scs.expected);
	    outgoing[peer].elm[curElm].address =
	      outgoing[peer].elm[curElm].scs.ptr;
	    outgoing[peer].elm[curElm].nDmaIssued=0;
	    goto case_203;
	  case 2: /* buffered   -> unbuffered */
	    outgoing[peer].elm[curElm].address =
	      (void *)(REQUEST&MASK_ADDRESS);
	    outgoing[peer].elm[curElm].nPacked    = 0;
	    outgoing[peer].elm[curElm].nDmaCompleted = 0;
	    goto case_201;
	  case 3: /* unbuffered -> unbuffered */
	    {
	      void *src = outgoing[peer].elm[curElm].scs.ptr;
	      void *dst = (void *)(REQUEST&MASK_ADDRESS);
	      int   len = outgoing[peer].elm[curElm].scs.expected;

	      assert((len&3)==0);

	      cache_copyback(src,len);
	      
        /*fprintf(stderr,"3:LADDR:%p\n",dst);/**/
	      dma_issue_put(dst,src,len,
			    &emb_personal[peer]->OUTGOING_CONFIRM[emb_procNo],1,
			    0,0);
	      
	      return 299; /* wait for complete reception */
	    }
	  default:
	    assert(0);
	  }	
      } /* end of case: 200 */
      assert(0);

      /*
       * "the other side" is unbuffered, while we must work buffered.
       * so let's pack those bytes, and drop 'em at the other side
       * contigiously in 4K blocks!
       */
      
    case_201: /* buffered->unbuffered */
    case 201:
      {
	int busy_buffers,bufNo;
	int lastBuf;

	/* can I pack an outgoing fragment? */
	
	busy_buffers =
	  outgoing[peer].elm[curElm].nPacked-outgoing[peer].elm[curElm].nDmaCompleted;

	assert(busy_buffers>=0);
	assert(busy_buffers<=BPQ);

	stall_detect(1001);

	if (busy_buffers==BPQ) /* all buffers in use! */
	  return 201;

	stall_progress(1001);

	/* we can pack a buffer! make it so ... */
	bufNo = outgoing[peer].elm[curElm].nPacked % BPQ;

	outgoing[peer].LOCAL_BYTES[bufNo] =
	  do_memcpy_stride_to_linear(&outgoing[peer].elm[curElm].scs,
				     (unsigned char *)outgoing[peer].LOCAL[bufNo],
				     BUFFER_SIZE);

	++outgoing[peer].elm[curElm].nPacked;

	/* do cache_copyback... */

	cache_copyback(outgoing[peer].LOCAL[bufNo],
		       outgoing[peer].LOCAL_BYTES[bufNo]);

	/* send it on its way! */

	lastBuf = outgoing[peer].elm[curElm].scs.finished;

	/*fprintf(stderr,"201:LADDR:%p\n",outgoing[peer].elm[curElm].address);/**/
	if (!lastBuf)
	  {
	    dma_issue_put(outgoing[peer].elm[curElm].address,
			  outgoing[peer].LOCAL[bufNo],
			  outgoing[peer].LOCAL_BYTES[bufNo],
			  0,0,
			  &outgoing[peer].elm[curElm].nDmaCompleted,
			  outgoing[peer].elm[curElm].nPacked);

	    /* this was not the last fragement. Same state next time... */
	    outgoing[peer].elm[curElm].address+=outgoing[peer].LOCAL_BYTES[bufNo];
	    return 201;
	  }
	else
	  {
	    dma_issue_put(outgoing[peer].elm[curElm].address,
			  outgoing[peer].LOCAL[bufNo],
			  outgoing[peer].LOCAL_BYTES[bufNo],
			  &emb_personal[peer]->OUTGOING_CONFIRM[emb_procNo],1,
			  0,0);

	    /* this _was_ the last fragment. Wait for confirmation. */
	    return 299;
	  }	  
      } /* end of case 201: send buffered -> unbuffered */

    case_202: /* buffered-to-buffered */
    case 202:
      {
	int bufNo;
	int nProcessed;
	int lastBuf;
	
	/* can I pack and issue another buffer? */

	nProcessed = 
	  pio_read_local(&emb_myPersonal->INCOMING_REQUEST[peer])-1;

	stall_detect(1002);

	if (outgoing[peer].elm[curElm].nDmaIssued-nProcessed>=BPQ)
	  return 202; /* no go... no receive buffer is available */

	stall_progress(1002);
	
	/* yes... pack */

	bufNo = outgoing[peer].elm[curElm].nDmaIssued % BPQ;

	outgoing[peer].LOCAL_BYTES[bufNo] =
	  do_memcpy_stride_to_linear(&outgoing[peer].elm[curElm].scs,
				     (unsigned char *)outgoing[peer].LOCAL[bufNo],
				     BUFFER_SIZE);

	++outgoing[peer].elm[curElm].nDmaIssued;

	/* do cache_copyback... */

	cache_copyback(outgoing[peer].LOCAL[bufNo],outgoing[peer].LOCAL_BYTES[bufNo]);

	/* send it on its way! */

	lastBuf = outgoing[peer].elm[curElm].scs.finished;

        /*fprintf(stderr,"202:LADDR:%p\n",outgoing[peer].REMOTE[bufNo]);/**/
	dma_issue_put(outgoing[peer].REMOTE[bufNo],
		      outgoing[peer].LOCAL[bufNo],
		      outgoing[peer].LOCAL_BYTES[bufNo],
		      &emb_personal[peer]->OUTGOING_CONFIRM[emb_procNo],
		      outgoing[peer].elm[curElm].nDmaIssued,
		      0,0);
	
	/* this was not the last fragement. Same state next time... */

	if (lastBuf==0)
	  return 202;

	return 299;
      }

      /* unbuffered -> buffered */

    case_203:
    case 203:
      {
	int bufNo;
	int nProcessed;
	int lastBuf;
	int xferSize;

	/* can I issue another buffer? */

	nProcessed = 
	  pio_read_local(&emb_myPersonal->INCOMING_REQUEST[peer])-1;

	if (stall_detect(1017))
	  {
	    pprintf("sender: nProcessed=%d remaining=%d\n",nProcessed,
		    outgoing[peer].elm[curElm].scs.remaining
		    );
	  }

	if (outgoing[peer].elm[curElm].nDmaIssued-nProcessed>=BPQ)
	  return 203; /* no go... no receive buffer is available */

	stall_progress(1017);

	/* yes...  */

	bufNo = outgoing[peer].elm[curElm].nDmaIssued % BPQ;

	++outgoing[peer].elm[curElm].nDmaIssued;

	/* send it on its way! */

	xferSize = BUFFER_SIZE;
	if (outgoing[peer].elm[curElm].scs.remaining<xferSize)
	  {
	    xferSize = outgoing[peer].elm[curElm].scs.remaining;
	  }

	
	outgoing[peer].elm[curElm].scs.remaining-=xferSize;

	lastBuf = (int)(outgoing[peer].elm[curElm].scs.remaining) == 0;

        /*fprintf(stderr,"203:LADDR:%p,DATA:%d\n",outgoing[peer].elm[curElm].address,*((int *)outgoing[peer].elm[curElm].address));/**/
	dma_issue_put(outgoing[peer].REMOTE[bufNo],
		      outgoing[peer].elm[curElm].address,
		      xferSize,
		      &emb_personal[peer]->OUTGOING_CONFIRM[emb_procNo],
		      outgoing[peer].elm[curElm].nDmaIssued,
		      0,0);

	outgoing[peer].elm[curElm].address+=xferSize;

	if (0)
	  pprintf("REMAINING: %d LASTBUF=%d!\n",outgoing[peer].elm[curElm].scs.remaining,lastBuf);

	/* this was not the last fragment. Same state next time... */
	
	if (lastBuf==0)
	  return 203;


	return 299;
      }

    case 299: /* wait for confirmation of complete reception */
      {
	stall_detect(1004);

	if (pio_read_local(&emb_myPersonal->INCOMING_REQUEST[peer])!=0)
	  return 299; /* no complete reception yet. */


	stall_progress(1004);

	/* Received confirmation; let it be known that we've seen this! */
	pio_write_remote(&emb_personal[peer]->OUTGOING_CONFIRM[emb_procNo],0);

	return 0; /* all done */
      }

      /*********************************
       * states 300-303: send HOST->TM *
       *********************************/

    case_300:
    case 300:
      {
	unsigned long REQUEST;
	/* wait for request ... */
	REQUEST = pio_read_local(&emb_myPersonal->INCOMING_REQUEST[peer]);

	stall_detect(1005);

	if (REQUEST==0)
	  return 300;

	stall_progress(1005);

	goto case_302;
      }

    case_302:
    case 302:
      {
	int bufNo;
	int nProcessed;
	int lastBuf;
	
	/* can I pack another buffer? */

	nProcessed = 
	  pio_read_local(&emb_myPersonal->INCOMING_REQUEST[peer])-1;

	stall_detect(1006);

	if (outgoing[peer].elm[curElm].nPacked-nProcessed>=BPQ)
	  return 302; /* no go... no receive buffer is available */

	stall_progress(1006);

	/* yes... pack */

	bufNo = outgoing[peer].elm[curElm].nPacked % BPQ;


	outgoing[peer].LOCAL_BYTES[bufNo] =
	  do_memcpy_stride_to_linear(&outgoing[peer].elm[curElm].scs,
				     outgoing[peer].LOCAL[bufNo],
				     BUFFER_SIZE);

	++outgoing[peer].elm[curElm].nPacked;

	/* do cache_copyback... */

	cache_copyback(outgoing[peer].LOCAL[bufNo],
		       outgoing[peer].LOCAL_BYTES[bufNo]);

	/* make known to the RECEIVER that the buffer is ready to be fetched. */

	pio_write_remote(&emb_personal[peer]->OUTGOING_CONFIRM[emb_procNo],
			 outgoing[peer].elm[curElm].nPacked);

	/* this was not the last fragment. Same state next time... */

	lastBuf = outgoing[peer].elm[curElm].scs.finished;

	if (lastBuf)
	    return 304;

	return 302;
      }

    case 304:
      {

	stall_detect(1007);

	if (pio_read_local(&emb_myPersonal->INCOMING_REQUEST[peer])!=0)
	  return 304;

	stall_progress(1007);
	pio_write_remote(&emb_personal[peer]->OUTGOING_CONFIRM[emb_procNo],0);
	return 0;
      }

    default:
      assert(0);
    }
  assert(0);
  return -1; /* unreachable, to make tmcc happy */
}

/*****************************************************************************
 *                                                                           *
 *                           emb_process_incoming()                          *
 *                                                                           *
 *****************************************************************************/

static int emb_process_incoming(int peer, int state, int curElm)
{
  if (SHOW_QUEUE)
    pprintf("emb_process_incoming: peer = %d state = %d\n",peer,state);
  
  switch(state)
    {
    case 0: /* INITIAL STATE */
      if (emb_procNo==peer) /* send-to-self? */
	{
	  /* this case is handled by the "outgoing" queue. */
	  return 0;
	}
      switch(emb_procType[emb_procNo])
	{
	case TRIMEDIA:
	  switch(emb_procType[peer])
	    {
	    case TRIMEDIA:
	      incoming[peer].elm[curElm].nDmaCompleted = 0;
	      incoming[peer].elm[curElm].nPacked       = 0;
	      incoming[peer].elm[curElm].nUnpacked     = 0;
	      incoming[peer].elm[curElm].nConfirmed    = 0;
	      incoming[peer].elm[curElm].nDmaIssued    = 0;  
	      goto case_200; /* TRIMEDIA <- TRIMEDIA */

	    case PENTIUM:
	      /* check if my area is continuous */
	      pio_write_remote(&emb_personal[peer]->INCOMING_REQUEST[emb_procNo],MASK_REQUEST);
	      incoming[peer].elm[curElm].nDmaCompleted = 0;
	      incoming[peer].elm[curElm].nPacked       = 0;
	      incoming[peer].elm[curElm].nUnpacked     = 0;
	      incoming[peer].elm[curElm].nConfirmed    = 0;
	      incoming[peer].elm[curElm].nDmaIssued    = 0;  
	      if (incoming[peer].elm[curElm].scs.contiguous)
		{
		  cache_copyback(incoming[peer].elm[curElm].scs.ptr,
				 incoming[peer].elm[curElm].scs.expected);
		  cache_invalidate(incoming[peer].elm[curElm].scs.ptr,

				   incoming[peer].elm[curElm].scs.expected);
		  incoming[peer].elm[curElm].address = incoming[peer].elm[curElm].scs.ptr;
		  goto case_303; /* TRIMEDIA.U <- HOST.B */
		}
	      else
		{
		  goto case_302; /* TRIMEDIA.B <- HOST.B */
		}
	    default:
	      assert(0);
	    }

	case PENTIUM:
	  switch(emb_procType[peer])
	    {
	    case TRIMEDIA:
	      incoming[peer].elm[curElm].nDmaCompleted = 0;
	      incoming[peer].elm[curElm].nPacked       = 0;
	      incoming[peer].elm[curElm].nUnpacked     = 0;
	      incoming[peer].elm[curElm].nConfirmed    = 0;
	      incoming[peer].elm[curElm].nDmaIssued    = 0;  
	      pio_write_remote(&emb_personal[peer]->INCOMING_REQUEST[emb_procNo],MASK_REQUEST);
	      goto case_202; /* HOST <- TRIMEDIA */
	    default:
	      assert(0);
	    }
	default:
	  assert(0);
	}

      /******************************
       * states 2xx: receive TM<-TM *
       ******************************/

    case_200:
    case 200:

      /*
       * Both me and the other side are a Trimedia, so the other side might
       * be able to put stuff in my buffer directly. So first, let me check
       * if my receive buffer is contiguous.
       */

      if (incoming[peer].elm[curElm].scs.contiguous)
	{
	  unsigned long REQUEST;

	  /* yes, it's contiguous.
	   * Make the address known to the other side, and request a direct
	   * transfer. */
	  
	  REQUEST = ((unsigned long)incoming[peer].elm[curElm].scs.ptr);
	  assert(REQUEST);
	  assert((REQUEST&3)==0);
	  REQUEST|=(MASK_REQUEST|MASK_ZEROCOPY);

	  /* copyback the memory, in case it's not cache aligned ... */

	  cache_copyback(incoming[peer].elm[curElm].scs.ptr,
			 incoming[peer].elm[curElm].scs.expected);
	  
	  /* post request */

	  incoming[peer].elm[curElm].nConfirmed=0;
	  pio_write_remote(&emb_personal[peer]->INCOMING_REQUEST[emb_procNo],
			   REQUEST);

	  return 298; /* next state: wait for completion. */
	}
      else
	{
	  unsigned long REQUEST;

	  /*
	   * unfortunately, zero-copy is not possible with me. I will have to resort to
	   * a flow-controlled buffer transfer. Request this.
	   */

	  REQUEST = MASK_REQUEST;

	  pio_write_remote(&emb_personal[peer]->INCOMING_REQUEST[emb_procNo],
		    REQUEST);

	  incoming[peer].elm[curElm].nConfirmed=0;
	  return 202;

	}

      /* now process receives as they come in. */

    case_202:
    case 202: /* buffered receive */
      {

	unsigned long nDma;

	nDma = pio_read_local(&emb_myPersonal->OUTGOING_CONFIRM[peer]);

	if (stall_detect(2001))
	  {
	    pprintf("peer=%d (nDMA=%lu nConfirmed=%lu)\n",
		   peer,nDma,incoming[peer].elm[curElm].nConfirmed);
	  }

	assert(incoming[peer].elm[curElm].nConfirmed<=nDma);
	if (nDma==incoming[peer].elm[curElm].nConfirmed)
	  return 202; /* no work to be done. */

	stall_progress(2001);

	/* cache-invalidate the buffer to unpack */

	cache_invalidate(incoming[peer].LOCAL[incoming[peer].elm[curElm].nConfirmed%BPQ],BUFFER_SIZE);

	/* stride-unpack a buffer */

	incoming[peer].LOCAL_BYTES[incoming[peer].elm[curElm].nConfirmed%BPQ] =
	  do_memcpy_linear_to_stride(&incoming[peer].elm[curElm].scs,
				     incoming[peer].LOCAL[incoming[peer].elm[curElm].nConfirmed%BPQ],BUFFER_SIZE);

	incoming[peer].elm[curElm].nConfirmed++;

	/* confirm this to the other side! */

	if (!incoming[peer].elm[curElm].scs.finished)
	  {
	    pio_write_remote(&emb_personal[peer]->INCOMING_REQUEST[emb_procNo],incoming[peer].elm[curElm].nConfirmed+1);

	    return 202;
	  }
	else
	  {
	    pio_write_remote(&emb_personal[peer]->INCOMING_REQUEST[emb_procNo],0);
	    return 299;
	  }
      }

    case 298:
      {
	/* waiting for the completion of an incoming contiguous transfer. */
	
	if (stall_detect(2002))
	  {
	    unsigned long V;
	    V = pio_read_local(&emb_myPersonal->OUTGOING_CONFIRM[peer]);
	    pprintf("incoming-298 peer=%d curElm=%d outgoing-confirm: %lu (waiting for non-zero)\n",curElm,peer,V);
	  }

	if (pio_read_local(&emb_myPersonal->OUTGOING_CONFIRM[peer])==0)
	  return 298;

	stall_progress(2002);

	pio_write_remote(&emb_personal[peer]->INCOMING_REQUEST[emb_procNo],0);

	/*
	 * invalidate the received memory area.
	 * CHECK: THIS MAY INVALIDATE TOO MUCH!
	 */
	
	cache_invalidate(incoming[peer].elm[curElm].scs.ptr,
			 incoming[peer].elm[curElm].scs.expected);
	
	return 299;
      }

    case 299:
      {
	stall_detect(2003);

	if (pio_read_local(&emb_myPersonal->OUTGOING_CONFIRM[peer])!=0)
	  return 299;


	stall_progress(2003);

	return 0; /* all done */
      }

      /* case 3xx: receive TM <- Host */

    case_302:
    case 302: /* buffered receive from host, I perform the DMA */
      {

	unsigned long nPacked;

	/* are remote buffers ready-for-pickup? */
	nPacked = pio_read_local(&emb_myPersonal->OUTGOING_CONFIRM[peer]);

	stall_detect(2004);
	
	while(incoming[peer].elm[curElm].nDmaIssued<nPacked)
	  {
	    /* issue DMA for remote buffers */
	    dma_issue_get(incoming[peer].LOCAL [incoming[peer].elm[curElm].nDmaIssued%BPQ],
			  incoming[peer].REMOTE[incoming[peer].elm[curElm].nDmaIssued%BPQ],
			  BUFFER_SIZE,
			  0,0,
			  &incoming[peer].elm[curElm].nDmaCompleted,incoming[peer].elm[curElm].nDmaIssued+1);
	    incoming[peer].elm[curElm].nDmaIssued++;
	  }

	if (incoming[peer].elm[curElm].nUnpacked==incoming[peer].elm[curElm].nDmaCompleted)
	  return 302;

	stall_progress(2004);


	/* cache-invalidate the buffer to unpack */
	
	cache_invalidate(incoming[peer].LOCAL[incoming[peer].elm[curElm].nUnpacked%BPQ],BUFFER_SIZE);

	/* stride-unpack a buffer */

	incoming[peer].LOCAL_BYTES[incoming[peer].elm[curElm].nUnpacked%BPQ] =
	  do_memcpy_linear_to_stride(&incoming[peer].elm[curElm].scs,
				     incoming[peer].LOCAL[incoming[peer].elm[curElm].nUnpacked%BPQ],
				     BUFFER_SIZE);

	incoming[peer].elm[curElm].nUnpacked++;

	/* confirm this to the other side! */

	if (!incoming[peer].elm[curElm].scs.finished)
	  {
	    pio_write_remote(&emb_personal[peer]->INCOMING_REQUEST[emb_procNo],incoming[peer].elm[curElm].nUnpacked+1);
	    return 302;
	  }
	else
	  {
	    pio_write_remote(&emb_personal[peer]->INCOMING_REQUEST[emb_procNo],0);
	    return 299;
	  }
      }

    case_303:
    case 303: /* UNbuffered receive from host, I perform the DMA */
      {

	unsigned long nPacked;

	/* are remote buffers ready-for-pickup? */
	nPacked = pio_read_local(&emb_myPersonal->OUTGOING_CONFIRM[peer]);

	stall_detect(2005);

	while(incoming[peer].elm[curElm].nDmaIssued<nPacked)
	  {
	    int sz;
	    int last;

	    sz = incoming[peer].elm[curElm].scs.remaining;
	    if (sz>BUFFER_SIZE) sz=BUFFER_SIZE;

	    /* issue DMA for remote buffers */

	    incoming[peer].elm[curElm].scs.remaining-=sz;

	    last = incoming[peer].elm[curElm].scs.remaining==0;

	    if (last)
	      {
		dma_issue_get(incoming[peer].elm[curElm].address,
			      incoming[peer].REMOTE[incoming[peer].elm[curElm].nDmaIssued%BPQ],
			      sz,
			      &emb_personal[peer]->INCOMING_REQUEST[emb_procNo],0,
			      0,0);
		stall_progress(2005);
		return 304;
	      }
	    else
	      {
		dma_issue_get(incoming[peer].elm[curElm].address,
			      incoming[peer].REMOTE[incoming[peer].elm[curElm].nDmaIssued%BPQ],
			      sz,
			      &emb_personal[peer]->INCOMING_REQUEST[emb_procNo],
			      incoming[peer].elm[curElm].nDmaIssued+2,
			      0,0);

		incoming[peer].elm[curElm].address+=sz;
		incoming[peer].elm[curElm].nDmaIssued++;
	      }
	  }
	stall_progress(2005);
	return 303;
      }

    case 304:
      {
	stall_detect(2006);
	if (pio_read_local(&emb_myPersonal->OUTGOING_CONFIRM[peer])!=0)
	  return 304;
	stall_progress(2006);
	return 0;
      }

    default:
      assert(0);
    }
  assert(0);
  return -1; /* unreachable, to make tmcc happy */
}


/*****************************************************************************
 *                                                                           *
 *                               emb_fence()                                 *
 *                                                                           *
 *****************************************************************************/

void emb_check(char *str)
{
  if (incoming[emb_procNo].nElm>=100)
    {
      printf("ERROR: ndims corrupted (%s)\n",str);
      assert(0);
    }
}

void emb_fence(void)
{
  int i,flag;
  int ALIVE;

  ++FENCE_ENTER;

  /* our job here is to process all elements in our incoming and outgoing queues.
   * Jobs should be handled in-order, for each queue that is.
   *
   * Queues are handled independently of all other queues.
   *  
   */

  if (0)
    {
      pprintf("emb_fence ENTER\n");
    }

  assert(incoming[emb_procNo].nElm<100);

  /*********************/
  /**    scheduler    **/
  /*********************/

  /* clear the "current element" iterator and "state" of all queues. */

  for(i=0;i<emb_nProcs;i++)
    {
      incoming[i].curElm = 0; incoming[i].state = 0;
      outgoing[i].curElm = 0; outgoing[i].state = 0;
    }

  if (SHOW_PRESCHEDULE)
  {
    int i,j,pr;
    int okflag = 1;

    for(pr=0;pr<emb_nProcs;pr++)
      {
	if (emb_procNo==pr)
	  {
	    for(i=0;i<emb_nProcs;i++)
	      {
		for(j=0;j<outgoing[i].nElm;j++)
		  {
		    pprintf("outgoing[%d]: scs %p ndims %10d expected %10d contiguous %10d pointer %p\n",i,
			   &outgoing[i].elm[j].scs,
			   outgoing[i].elm[j].scs.ndims,
			   outgoing[i].elm[j].scs.expected,
			   outgoing[i].elm[j].scs.contiguous,
			   outgoing[i].elm[j].scs.ptr);

		    if (outgoing[i].elm[j].scs.expected<=0) okflag = 0;
		    if (outgoing[i].elm[j].scs.expected>=10000000) okflag = 0;

		  }

		for(j=0;j<incoming[i].nElm;j++)
		  {
		    pprintf("incoming[%d]: scs %p ndims %10d expected %10d contiguous %10d pointer %p  nelms=%d\n",i,
			   &incoming[i].elm[j].scs,
			   incoming[i].elm[j].scs.ndims,
			   incoming[i].elm[j].scs.expected,
			   incoming[i].elm[j].scs.contiguous,
			   incoming[i].elm[j].scs.ptr,incoming[i].nElm);
		    
		    if (outgoing[i].elm[j].scs.expected<=0) okflag = 0;
		    if (outgoing[i].elm[j].scs.expected>=10000000) okflag = 0;

		  }
		
	      }
	    fflush(stdout);
	  }
	emb_barrier();
      }
    if (SANITY_CHECK && (okflag==0))
      {
	pprintf("processor fails sanity check!\n");
	sleep(1);
	exit(1);
      }
  }

  ALIVE = 0;
  do {
    
    flag = 0;

    /* process incoming queues, round robin */
    for(i=0;i<emb_nProcs;i++)
      if (incoming[i].curElm<incoming[i].nElm)
	{
	  int old, new;

	  /* processing incoming queue element #i */

	  old = incoming[i].state;

	  incoming[i].state =
	    emb_process_incoming(i,incoming[i].state,incoming[i].curElm);

	  new = incoming[i].state;


	  if (SHOW_STATE_TRANSITIONS)
	    if ((old != new) || (new==0))
	      pprintf("incoming[%d] element %3d of %3d    %5d -> %5d     fence: %10lu\n",i,incoming[i].curElm,incoming[i].nElm, old, new, FENCE_ENTER);
	  
	  if (incoming[i].state==0)
	    incoming[i].curElm++;
	  else
	    flag=1;

	  if (incoming[i].curElm<incoming[i].nElm)
	    flag=1;
	}

    /* process outgoing queues, round robin */
    for(i=0;i<emb_nProcs;i++)
      if (outgoing[i].curElm<outgoing[i].nElm)
	{
	  int old,new;
	  /* processing outgoing queue element #i */

	  old = outgoing[i].state;

	  outgoing[i].state =
	    emb_process_outgoing(i,outgoing[i].state,outgoing[i].curElm);

	  new = outgoing[i].state;

	  if (SHOW_STATE_TRANSITIONS)
	    if ((old != new) || (new==0))
	      pprintf("outgoing[%d] element %3d of %3d    %5d -> %5d     fence: %10lu\n",i,outgoing[i].curElm,outgoing[i].nElm, old, new,FENCE_ENTER);
	  
	  
	  if (outgoing[i].state==0)
	    {
	      if (0)
		pprintf("done outgoing peer=%d elm=%d nElm=%d\n",
		     i,outgoing[i].curElm,outgoing[i].nElm);
	      outgoing[i].curElm++;
	    }

	  if (outgoing[i].curElm<outgoing[i].nElm)
	    flag=1;
	}    

    if(0)
    if (++ALIVE==500000)
      {
	int i;
	pprintf("ALIVE!\n");
	fence_info();
	for(i=0;i<emb_nProcs;i++)
	  {
	    pprintf("incoming[%d] = %d/%d  STATE=%d\n",i,incoming[i].curElm,incoming[i].nElm,incoming[i].state);
	    pprintf("outgoing[%d] = %d/%d  STATE=%d\n",i,outgoing[i].curElm,outgoing[i].nElm,outgoing[i].state);
	  }
	ALIVE=0;
      }
    
  } while (flag==1);
  
  /* empty incoming and outgoing queues */

  for(i=0;i<emb_nProcs;i++)
    {
      outgoing[i].nElm = 0;
      incoming[i].nElm = 0;
    }

  if (0)
    {
      pprintf("emb_fence LEAVE\n");
    }

#ifdef __TCS__
  while(dma_queue.in_use!=0)
    {
      if (stall_detect(1111))
	pprintf("dma-queue in-use: %d\n",dma_queue.in_use);
    }
  stall_progress(1111);
#endif

  ++FENCE_LEAVE;

}

volatile int z=0;

void test_barrier(void)

{
  int sync,i;

  srand(emb_procNo);

  emb_myPersonal->BARCOUNT.value=0;
  while(1)
    {
      for(sync=1;sync<=100000;sync++)
	{
	  
	  emb_myPersonal->BARCOUNT.value++;

	  cache_copyback(emb_myPersonal,4096);
	  for(i=1;i<=10;i++)
	    {
	      while(rand()%100)
		++z;
	      emb_barrier();
	    }
	  
	  if (emb_procNo==0 && (emb_myPersonal->BARCOUNT.value)%10000==0)
	    {
	      pprintf("0: %lu 1: %lu\n",
		      emb_personal[0]->BARCOUNT.value,
		      emb_personal[1]->BARCOUNT.value
		      );
	    }
	  emb_barrier();
	}
    }
}


/*****************************************************************************
 *                                                                           *
 *                                 emb_init()                                *
 *                                                                           *
 *****************************************************************************/

int emb_init(const int argc, char **argv, const int nProcs, const int useHost)
{
  unsigned long bootpage_addr;

  dma_init();

#ifdef __TCS__
  if (CACHE_DISABLE)
    {
      pprintf("disabling cache...\n");
      cache_copyback(MMIO(DRAM_BASE),MMIO(DRAM_LIMIT)-MMIO(DRAM_BASE));
      MMIO(DRAM_CACHEABLE_LIMIT) = MMIO(DRAM_BASE);
      pprintf("disabling cache done.\n");
    }
#endif

  /* check whether the declared page-structures fit in one page */
  
  assert(sizeof(BootPage)<=4096);
  assert(sizeof(PersonalPage)<=4096);

  if (VERBOSE)
    pprintf("Entering emb_init\n");

#ifdef __GNUC__
  {
    int i;
    char TMIMAGE[1000];
    
    /* give up the root privileges ASAP */

    set_root_privileges(0);
    
    /* get "boot-page", which is used to bootstrap communication */
    get_locked_page((void **)&emb_bootPage, &bootpage_addr);
    
    pio_write_local((CacheLineLong *)&emb_bootPage->NPROCS,nProcs);

    /* the name of the Trimedia executable... */
    sprintf(TMIMAGE,"%s-trimedia.out",argv[0]);

    {
      int i;
      for(i=1;i<argc;i++)
	{
	  sprintf(TMIMAGE+strlen(TMIMAGE)," %s",argv[i]);
	}
    }

    if (useHost) /* Host + (NPROCS-1) Trimedias */
      {
	emb_procNo = 0; /* Host is processor zero. */

	/* startup processor TM[i-1] as processor i */
	for(i=1;i<nProcs;i++)
	    kickoff(i-1,TMIMAGE,bootpage_addr,i,&BASEPTR[i]);

      }
    else /* NPROCS Trimedia's, Host idle. */
      {
	emb_procNo = -1; /* Host will be idle */

	/* startup processor TM[i] as processor i */
	for(i=0;i<nProcs;i++)
	    kickoff(i,TMIMAGE,bootpage_addr,i,&BASEPTR[i]);
      }

    if (VERBOSE)
      {
	pprintf("init: host finished kickoffs...\n");
      }
  }
#endif

#ifdef __TCS__

  /* We're on the Trimedia. So, the host has done a kickoff() for
     us, providing us with parsable command line parameters for boot-
     strapping, and our main() is now executing emb_init(). */


  /* We will now establish three ridiculously important variables:
     - our processor number (a command line parameter)
     - the bus address of the "BootPage" (a command line parameter)
     - the total number-of-processors (on the boot-page) */

  {
    int i;
    emb_procNo = -1;
    bootpage_addr = 0;
    for(i=0;i<argc;i++)
      {
	sscanf(argv[i],"--procno=%d",&emb_procNo);
	sscanf(argv[i],"--bootpage=%p",&bootpage_addr);
      }

    assert(emb_procNo!=-1);
    assert(bootpage_addr!=0);

    emb_bootPage = (BootPage *)bootpage_addr;
  }

#endif

  if (VERBOSE)
    {
      pprintf("init: processor %d in init...\n",emb_procNo);
    }
 
  emb_nProcs = pio_read_remote((CacheLineLong *)&emb_bootPage->NPROCS);
  
  /* This is the point where both the host and Trimedias arrive,
     with an accessable boot-page, knowing their own processor ID (emb_procNo)
     and the total number-of-processors (emb_nProcs). */
     
  if (emb_procNo!=-1) /* if I am supposed to be idle, skip this: */
    {
      /* sanity checks on variables */

      assert(emb_nProcs>0);
      assert(emb_nProcs<=MAX_PROCESSORS);
      assert(emb_procNo>=0);
      assert(emb_procNo<emb_nProcs);
      
      /* now we set up one "personal page" for outside access, and NBUFFERS
       * pages to be used as buffers...
       */
      
#ifdef __GNUC__
      {
	int i;
	get_locked_page((void **)&emb_myPersonal,
			(unsigned long *)&emb_bootPage->PAGES[emb_procNo][0]);
	assert(emb_myPersonal);
	memset((char *)emb_myPersonal,0,sizeof(PersonalPage));
	pio_write_local(&emb_myPersonal->PROCTYPE,PENTIUM);
	for(i=0;i<BUFFERS_PER_PROCESSOR;i++)
	  {
	    get_locked_page((void**)&emb_myBuffer[i],
			  (unsigned long *)&emb_bootPage->PAGES[emb_procNo][1+i]);
	    assert(emb_myBuffer[i]);
	  }
      }
#endif
  
#ifdef __TCS__
      {
	int i;
	emb_myPersonal = cache_malloc(sizeof(PersonalPage));
	assert(emb_myPersonal);
	memset((char *)emb_myPersonal,0,sizeof(PersonalPage));
	pio_write_local(&emb_myPersonal->PROCTYPE,TRIMEDIA);
	cache_copyback(emb_myPersonal,sizeof(PersonalPage));

	pio_write_remote((CacheLineLong *)&emb_bootPage->PAGES[emb_procNo][0],(unsigned long)emb_myPersonal);
	for(i=0;i<BUFFERS_PER_PROCESSOR;i++)
	  {
	    emb_myBuffer[i] = cache_malloc(BUFFER_SIZE);
	    assert(emb_myBuffer[i]);
	    memset(emb_myBuffer[i],0,BUFFER_SIZE);
	    pio_write_remote((CacheLineLong *)&emb_bootPage->PAGES[emb_procNo][1+i],(unsigned long)emb_myBuffer[i]);
	  }
      }
#endif
      
      if (VERBOSE)
	pprintf("init: processor finished buffer allocation!\n");
      
      /* Flag the bootable page as "ready" */

      pio_write_remote((CacheLineLong *)&emb_bootPage->READY[emb_procNo],1);
      
      /* Walk all processors in the system.
       * Update local info as all processors go online.
       */
      
      {
	unsigned long i;
	for(i=0;i<emb_nProcs;i++)
	  {
	    int j;

	    while(pio_read_remote((CacheLineLong *)&emb_bootPage->READY[i])==0) ;

	    emb_personal[i] = (PersonalPage *)pio_read_remote((CacheLineLong *)&emb_bootPage->PAGES[i]);
	    assert(emb_personal[i]);

#ifdef __GNUC__
	    if (i==emb_procNo)
	      emb_personal[i] = emb_myPersonal;
	    else
	      {
		emb_personal[i] = (PersonalPage *)map2user(i,(char *)emb_personal[i]);
	      }
#endif

	    for(j=0;j<BUFFERS_PER_PROCESSOR;j++)
	      {
		if (i==emb_procNo)
		  {
		    emb_buffers[i][j] = (BufferPage *)pio_read_local((CacheLineLong *)&emb_bootPage->PAGES[i][1+j]);
		  }
		else
		  {
		    emb_buffers[i][j] = (BufferPage *)pio_read_remote((CacheLineLong *)&emb_bootPage->PAGES[i][1+j]);
		  }
		assert(emb_buffers[i][j]);
#ifdef __GNUC__
		if (i==emb_procNo)
		  emb_buffers[i][j] =  emb_myBuffer[j];
		else
		  {
		    emb_buffers[i][j] = (BufferPage *)map2user(i,(char *)emb_buffers[i][j]);
		  }
#endif
	      }
	    
	    if (i==emb_procNo)
	      {
		emb_procType[i] = pio_read_local(&emb_personal[i]->PROCTYPE);
	      }
	    else
	      {
		emb_procType[i] = pio_read_remote(&emb_personal[i]->PROCTYPE);
	      }

	  }
      }
      
      if (VERBOSE)
	pprintf("init: finished all-to-all phase\n");

      /* initialize send & receive queues */
      {
	int i,j;
	for(i=0;i<emb_nProcs;i++)
	  {
	    incoming[i].nElm = 0;
	    outgoing[i].nElm = 0;

	    incoming[i].n_dmaFrags = 0; incoming[i].n_packFrags = 0;
	    outgoing[i].n_dmaFrags = 0; outgoing[i].n_packFrags = 0;

	    /*
	     * a.incoming[b].LOCAL[j]  == b.outgoing[a].REMOTE[j]
	     * a.incoming[b].REMOTE[j] == b.outgoing[a].LOCAL[j]
	     */

	    for(j=0;j<BUFFERS_PER_QUEUE;j++)
	      {
		incoming[i].LOCAL[j]  =
		  emb_buffers[emb_procNo][2*BUFFERS_PER_QUEUE*i+j];
		outgoing[i].LOCAL[j]  =
		  emb_buffers[emb_procNo][2*BUFFERS_PER_QUEUE*i+BPQ+j];

		incoming[i].REMOTE[j] =
		  emb_buffers[i         ][2*BUFFERS_PER_QUEUE*emb_procNo+BPQ+j];
		outgoing[i].REMOTE[j] =
		  emb_buffers[i         ][2*BUFFERS_PER_QUEUE*emb_procNo+j];
	      }
	  }
      }

      timer_init();
      stall_init();
      if (VERBOSE)
      {
	int pr,i,j;
	
	for(pr=0;pr<emb_nProcs;pr++)
	  {
	    if (emb_procNo==pr)
	      {
		for(i=0;i<emb_nProcs;i++)
		  for(j=0;j<BPQ;j++)
		    pprintf("incoming[%d].LOCAL[%d]=%p incoming[%d].REMOTE[%d]=%p\n",i,j,incoming[i].LOCAL[j],i,j,incoming[i].REMOTE[j]);
		for(i=0;i<emb_nProcs;i++)
		  for(j=0;j<BPQ;j++)
		    pprintf("outgoing[%d].LOCAL[%d]=%p outgoing[%d].REMOTE[%d]=%p\n",i,j,outgoing[i].LOCAL[j],i,j,outgoing[i].REMOTE[j]);
	      }
	    emb_barrier();
	    
	  }
      }
      
      emb_barrier();

    } /* end if (procid>=0) */

  else

    { /* this is a (-1) processor */
#ifdef __TCS__
      assert(0);
#endif
#ifdef __GNUC__
      int i;

      for(i=1;i<=nProcs;i++)
	{
	  int pid;
	  pid = wait(0);
	  pprintf("Idle host detected termination of processor (pid=%d).\n",pid);
	}

      pprintf("Idle host detected termination of all processors, exiting.\n");
      exit(0);
#endif
    }

  emb_init_time = clock();

  if (VERBOSE)
    pprintf("Leaving init()\n");

  if (0)
    test_barrier();

  return 0;
}

/*****************************************************************************
 *                                                                           *
 *                                 emb_done()                                *
 *                                                                           *
 *****************************************************************************/

void emb_done(const int showSummary)
{
  
  double T;
  
  if (1)
    pprintf("entering emb_done\n");

  emb_barrier();

  if (1)
    pprintf("emb_done: first barrier passed.\n");

  emb_done_time = clock();
  
  T = (double)(emb_done_time - emb_init_time)/CLOCKS_PER_SEC;
  
  if (showSummary)
    {
      dma_Summary *S;
      
      S = cache_malloc(MAX_PROCESSORS*sizeof(dma_Summary));
      assert(S);
      
      memcpy(&S[emb_procNo],&dma_summary,sizeof(dma_Summary));
      
      if (emb_procNo!=0)
	emb_send_block(0,&S[emb_procNo],sizeof(dma_Summary),0);
      
      if (emb_procNo==0)
	{
	  int i;
	  for(i=1;i<emb_nProcs;i++)
	    {
	      emb_recv_block(i,&S[i],sizeof(dma_Summary),0);
	    }
	}
      
      emb_fence();
      
      if (emb_procNo==0)
	{
	  int i;
	  float B_put=0,B_get=0;
	  
	  printf("\n*** ENSEMBLE SUMMARY (%d processors) ***\n\n",emb_nProcs);
	  
	  for(i=0;i<emb_nProcs;i++)
	    {
	      char *pt;
	      
	      switch(emb_procType[i])
		{
		case PENTIUM : pt = "Host"; break;
		case TRIMEDIA: pt = "Trimedia"; break;
		default:
		  assert(0);
		  exit(1);
		}
	      
	      printf("  processor #%d (%s)\n",i,pt);
	      printf("    type         operations   total bytes \n");
	      printf("    ------------ ------------ ------------\n");
	      printf("    dma-put     %12.0f %12.0f\n", S[i].N_put,S[i].B_put);
	      printf("    dma-get     %12.0f %12.0f\n", S[i].N_get,S[i].B_get);
	      printf("\n");
	      
	      B_put += S[i].B_put;
	      B_get += S[i].B_get;
	      
	    }
	  
	  printf("DMA bytes............: put %.0f, get %.0f, total %.0f\n",B_put,B_get,B_put+B_get);
	  printf("Time.................: %.3f sec\n",T);
	  printf("Utilized bandwidth...: %.3f MB/sec\n\n",((B_put+B_get)/1048576.0)/T);	      

	  cache_free(S);
      
	}
      
      emb_barrier();

      if (1)
	pprintf("emb_done: last barrier passed.\n");
      
      dma_done();
      timer_done();
      
    } /* end showSummary */

  if (0)
    if (emb_procNo==0)
      trace_print();
      
  if (1)
    pprintf("leaving emb_done.\n");
}
