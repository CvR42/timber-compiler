/* File: panda.cc
 *
 * Service functions for the panda version of the backend.
 */

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "vnusstd.h"
#include "vnusrtl.h"
#include "shape.h"
#include "vnusprocdefs.h"
#include "partsend.h"

extern "C" 
{
#include <pan_sys.h>
#include <pan_align.h>
#include <pan_timer.h>

#include <pan_mp.h>
#include <pan_comm_util.h>
#include <pan_util.h>
#ifdef TRACING
#include <pan_trace.h>
#endif
}

//#define DEBUG_PANDA /*Set this to enable debugging */

#ifdef TRACING
typedef struct ev_log {
    int dummy;
} ev_log_t;

trc_event_t start_bla;
trc_event_t stop_bla;
ev_log_t whatever;
#endif

typedef struct MP_HDR_T {
    int         iov_size;
    int         size;
} hdr_t, *hdr_p;

static int		mp_proto_offset;
static int		mp_proto_top;

static pan_mp_ack_t	ack_type = PAN_MP_NORMAL;


static hdr_p
mp_hdr(void *proto_stack)
{
    return (hdr_p)((char *)proto_stack + mp_proto_offset);
}

static void
mp_proto_init(void)
{
    int offset        = pan_mp_proto_offset();
    mp_proto_offset   = align_to(offset, hdr_t);
    mp_proto_top      = mp_proto_offset + sizeof(hdr_t);
}



static int		me;
static int		ncpu;
static int		msg_port;




typedef struct MSG_Q_ITEM msg_q_item_t, *msg_q_item_p;

struct MSG_Q_ITEM {
    void               *data;
    int                size;
    msg_q_item_p        next;
};

typedef struct MSG_Q {
    msg_q_item_p        front;
    msg_q_item_p        tail;
    pan_mutex_p         lock;
    pan_cond_p          got_some;
    int                 waiting;
} msg_q_t, *msg_q_p;

static msg_q_p *queues;

static void
msg_q_enq(msg_q_p q, void *data, int size)
{
    msg_q_item_p p;

#ifdef DEBUG_PANDA
    fprintf(stderr,"panda:msg_q_enq enqueuing msg\n");
#endif
    p = (msg_q_item_p)pan_malloc(sizeof(msg_q_item_t));
    p->data = data;
    p->size = size;
    p->next = NULL;

    pan_mutex_lock(q->lock);
    if (q->front == NULL)
        q->front = p;
    else
        q->tail->next = p;
    q->tail = p;
    if (q->waiting>0)
        pan_cond_signal(q->got_some);
    pan_mutex_unlock(q->lock);
}

static void
msg_q_deq(msg_q_p q, void **data, int *size)
{
    msg_q_item_p p;

#ifdef DEBUG_PANDA
    fprintf(stderr,"panda:msg_q_deq dequeuing msg\n");
#endif
#ifdef TRACING
	trc_event(start_bla, &whatever);
#endif
    pan_mutex_lock(q->lock);
    while ((p = q->front) == NULL)
    {
        ++q->waiting;
        pan_cond_wait(q->got_some);
        --q->waiting;
    }
    q->front = p->next;
    if (q->front == NULL)
        q->tail = NULL;
    pan_mutex_unlock(q->lock);
    
    *data = p->data;
    *size = p->size;
    pan_free(p);
#ifdef TRACING
	trc_event(stop_bla, &whatever);
#endif
}

static void
msg_q_init(msg_q_p q)
{
    q->front = NULL;
    q->lock = pan_mutex_create();
    q->got_some = pan_cond_create(q->lock);
    q->waiting = 0;
}

static void
msg_q_remove(msg_q_p q)
{
    pan_cond_clear(q->got_some);
    pan_mutex_clear(q->lock);
}

/*
 * Upcall done when message arrives
 */
static void
handle_msg(pan_msg_p msg, void *proto)
{
#ifdef DEBUG_PANDA
    fprintf(stderr,"panda:handle_msg\n");
#endif
    hdr_p hdr;

    hdr = mp_hdr(proto);
    int size = hdr->size;
    char *data = (char *)pan_malloc(size);
    pan_msg_consume(msg,data,size);
    pan_msg_clear(msg);
    msg_q_enq(queues[pan_msg_sender(msg)],data,size);
}






/*
 * The User interface
 *
 */

VnusInt thisProcessor()
{
    return me;
}

VnusBoolean isThisProcessor(const VnusInt procNo)
{
    return (me == procNo);
}

void
vnus_send_real(const int dest,const int size,const void *buf)
{
    pan_iovec_t iov[1];
    int send_ticket;
    void *proto_stack;
    hdr_p hdr;

#ifdef DEBUG_PANDA
    fprintf(stderr,"panda:vnus_send_real %d bytes\n",size);
#endif
    proto_stack = pan_proto_create(mp_proto_top);
    hdr = mp_hdr(proto_stack);
    hdr->iov_size = 1;
    hdr->size = size;
    
    //    Data is now allocated at receive side
    // CvR: the const_cast shuts up even the most picky compiler about
    // the casting away of const. The cast is necessary because Panda
    // is not very const correct.
    iov[0].data = const_cast<void *>(buf);
    iov[0].len = hdr->size;
    
#ifdef DEBUG_PANDA
    fprintf(stderr,"panda:vnus_send_real: calling pan_mp_send_sync\n");
#endif
    send_ticket = pan_mp_send_sync(dest,msg_port,iov,1,
				   proto_stack,mp_proto_top,ack_type);
#ifdef DEBUG_PANDA
    fprintf(stderr,"panda:vnus_send_real: calling pan_mp_send_finish\n");
#endif
    pan_mp_send_finish(send_ticket);
#ifdef DEBUG_PANDA
    fprintf(stderr,"panda:vnus_send_real: calling pan_proto_clear\n");
#endif
    pan_proto_clear(proto_stack);
#ifdef DEBUG_PANDA
    fprintf(stderr,"panda:vnus_send_real: finished\n");
#endif
}

void
vnus_receive(const int src,const int size,void *buf)
{
    void *data;
    int rcvdSize;

#ifdef DEBUG_PANDA
    fprintf(stderr,"panda:vnus_receive\n");
#endif
    msg_q_deq(queues[src],&data, &rcvdSize);
    if (size != rcvdSize)
    {
        printf("Whoops, size(%d) != rcvdSize(%d)!\n",size,rcvdSize);
    }
    memcpy(buf,data,size);
    pan_free(data);
}

void
vnus_broadcast( const int size, const void *buf )
{
    for (int i=0; i<ncpu; i++)
        vnus_send_real(i,size,buf);
}

/*
 * To be sure send at least 1 byte...
 */
void
vnus_barrier(void)
{
    char buf[1];
    if (me != 0)
    {
        vnus_send_real(0,1,buf);
        vnus_receive(0,1,buf);
    } else {
        for (int i=0; i<ncpu; i++)
            if (i != me)
                vnus_receive(i,1,buf);
        for (int i=0; i<ncpu; i++)
            if (i != me)
                vnus_send_real(i,1,buf);
    }
}

void vnus_exec_forkall( void (*body)( int p, int, char** ), int nproc, int argc, char** argv )
{

    pan_init(&argc, argv);
    pan_mp_init(&argc, argv);
    pan_util_init(&argc, argv);

#ifdef TRACING
    start_bla = trc_new_event(0, sizeof(ev_log_t),"start bla", "");
    stop_bla = trc_new_event(0, sizeof(ev_log_t),"stop bla", "");
#endif
    
    me = pan_my_pid();
    ncpu = pan_nr_processes();

    if( ncpu<nproc ){
	fprintf( stderr, "Program was compiled for %d processors, but there are only %d. Goodbye!\n", nproc, ncpu );
	exit( 1 );
    }
    if( ncpu>nproc ){
	fprintf( stderr, "Program was compiled for %d processors, but there are %d. Goodbye!\n", nproc, ncpu );
    }

    mp_proto_init();
    queues = (msg_q_p*)pan_malloc(ncpu*sizeof(msg_q_p));
    for (int i=0; i<ncpu; i++)
        queues[i] = (msg_q_p)pan_malloc(sizeof(msg_q_t));
    for (int i=0; i<ncpu; i++)
        msg_q_init(queues[i]);
    msg_port = pan_mp_register_port(handle_msg);

    pan_start();

    body(me, argc, argv);

#ifdef TRACING
    trc_flush();
#endif

    pan_mp_free_port(msg_port);
    for (int i=0; i<ncpu; i++)
        msg_q_remove(queues[i]);
    for (int i=0; i<ncpu; i++)
        pan_free(queues[i]);
    pan_free(queues);
        
    pan_util_end();
    pan_mp_end();
    pan_end();
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
