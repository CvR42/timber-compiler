/* File: pvm.cc
 *
 * Service functions for the pvm version of the backend.
 */

#include <stdio.h>

#include <assert.h>
#include <stdlib.h>

#include "vnusstd.h"
#include "vnusprocdefs.h"
#include "vnusrtl.h"
#include "shape.h"

#include "pvm3.h"

#include "partsend.h"

extern "C" int pvm_freezegroup(const char *group, int ntask);

//#define TRACE_ON

#define TAG_ANY -1
#define TAG_HEADER 1
#define TAG_MSG 2
#define TAG_RETURN 3

static VnusInt hiddenThisProcessor;

static const char my_pvm_group[] = "yabba";

#define MAX_PVM_ERROR 34
static char my_pvm_default_error[] = "Pvm:Unknown error!";
static const char* my_pvm_error[MAX_PVM_ERROR] = 
{
    "PvmOk",                   //0       /* Error 0 */
    "Unknown PVM error",       //-1
    "Bad Parameter",           //-2      /* Bad parameter */
    "Count mismatch",          //-3      /* Count mismatch */
    "Value too large",         //-4      /* Value too large */
    "End of buffer",           //-5      /* End of buffer */
    "No such host",            //-6      /* No such host */
    "No such file",            //-7      /* No such file */
    "Unknown PVM error",       //-8
    "Unknown PVM error",       //-9
    "Out of memory",           //-10     /* Malloc failed */
    "Unknown PVM error",       //-11
    "PvmBadMsg",               //-12     /* Can't decode message */
    "Unknown PVM error",       //-13
    "Cannot contact local daemon", //-14     /* Can't contact local daemon */
    "No current buffer",       //-15     /* No current buffer */
    "No such buffer",          //-16     /* No such buffer */
    "Null group name",         //-17     /* Null group name */
    "Already in group",        //-18     /* Already in group */
    "No such group",           //-19     /* No such group */
    "Not in group",            //-20     /* Not in group */
    "No such instance",        //-21     /* No such instance */
    "Host failed",             //-22     /* Host failed */
    "No parent task",          //-23     /* No parent task */
    "Not implemented",         //-24     /* Not implemented */
    "Pvmd system error",       //-25     /* Pvmd system error */
    "Version mismatch",        //-26     /* Version mismatch */
    "Out of resources",        //-27     /* Out of resources */
    "Duplicate host",          //-28     /* Duplicate host */
    "Cannot start pvmd",       //-29     /* Can't start pvmd */
    "Already in progress",     //-30     /* Already in progress */
    "No such task",            //-31     /* No such task */
    "No such entry",           //-32     /* No such entry */
    "Duplicate entry",         //-33     /* Duplicate entry */
};

static void my_print_error(const char *s, int err)
{
    const char *p;
    if ((err>0) || (err<(-MAX_PVM_ERROR)))
        p = my_pvm_default_error;
    else
        p = my_pvm_error[-err];
    fprintf(stderr, "PVM::error %s(%d) in %s\n",p,err,s);
    exit(1);
}


/*
 * The User interface
 *
 */

VnusInt thisProcessor()
{
    return hiddenThisProcessor;
}

VnusBoolean isThisProcessor(const VnusInt procNo)
{
    return (hiddenThisProcessor == procNo);
}

void vnus_barrier( void )
{
#ifdef TRACE_ON
    fprintf(stderr,"v_bar:1\n");
#endif
    int info = pvm_barrier( const_cast<char *>(my_pvm_group), pvm_gsize(const_cast<char *>(my_pvm_group)) );
    if (info<0)
        my_print_error("vnus_barrier",info);
}

/* Given a destination processor, the number
 * of bytes to send, and a pointer to a
 * buffer, send the elements to the given processor.
 *
 * Any problems are reported within this function, so there is
 * no result.
 */
void vnus_send_real( const int dest, const int size, const void *buf )
{
#ifdef TRACE_ON
    fprintf(stderr,"v_snd_real:1\n");
#endif
    int theDest = pvm_gettid(const_cast<char *>(my_pvm_group), dest);
    int info = pvm_initsend(PvmDataRaw);
    if (info<0)
	my_print_error("vnus_send_real",info);
    info = pvm_pkbyte(const_cast<char *>(static_cast<const char *>(buf)), size, 1);
    if (info<0)
	my_print_error("vnus_send_real",info);
    info = pvm_send(theDest, TAG_MSG);
    if (info<0)
	my_print_error("vnus_send_real",info);
}

/* Given a source processor, and the number of bytes to be received,
 * and a pointer to a
 * buffer, receive the data from the given processor.
 *
 * If the number of bytes received doesn't match the requested size
 * an error is reported.
 *
 * Any problems are reported within this function, so there is
 * no result.
 */
void vnus_receive( const int src, const int size, void *buf )
{
#ifdef DEBUG_VnusCommunication
    fprintf(stderr,"vnus_recv at %d from %d\n",thisProcessor(),src);
#endif
#ifdef TRACE_ON
    fprintf(stderr,"v_rcv:1\n");
#endif
    int theSrc = pvm_gettid(const_cast<char *>(my_pvm_group), src);
    int bytes, msgtag, tid;
    
    int bufinfo = pvm_recv(theSrc, TAG_MSG);
    if (bufinfo<0)
        my_print_error("vnus_recv",bufinfo);
    int info = pvm_bufinfo(bufinfo, &bytes, &msgtag, &tid);
    if (info<0)
        my_print_error("vnus_recv",info);
    if (size != bytes)
        my_print_error("vnus_recv",info);
    info = pvm_upkbyte((char*)buf, bytes, 1);
    if (info<0)
        my_print_error("vnus_recv",info);
}

#if 0 // Down with PVM broadcast!!!!!
void vnus_broadcast( const int size, const void *buf )
{
#ifdef DEBUG_VnusCommunication
    fprintf(stderr,"vnus_broadcast from %d\n",thisProcessor());
#endif
#ifdef TRACE_ON
    fprintf(stderr,"v_brd:1\n");
#endif
#if 0
    fprintf(stderr, "vb:%d %d %p %d\n",
            nelm,
            elmsz,
            buf,
            thisProcessor());
#endif
    int info = pvm_initsend(PvmDataRaw);
    if (info<0)
        my_print_error("vnus_broadcast",info);
    info = pvm_pkbyte((char*)buf, size, 1);
    if (info<0)
        my_print_error("vnus_broadcast",info);
    info = pvm_bcast(my_pvm_group, TAG_MSG);
    if (info<0)
        my_print_error("vnus_broadcast",info);
        // Send to self. HARHAR!!!
    int theDest = pvm_gettid(my_pvm_group, thisProcessor());
    info = pvm_send(theDest, TAG_MSG);
    if (info<0)
        my_print_error("vnus_broadcast:send",info);
}
#else
void vnus_broadcast( const int size, const void *buf )
{
    for (int i=0; i<numberOfProcessors; i++)
      vnus_send_real(i,size,buf);
}
#endif

void vnus_exec_forkall( void (*body)( int p, int, char** ), int nproc, int argc, char** argv )
{
    int mytid;                  /* my task id */
    int *tids;                  /* array of task id */
    int me;                     /* my process number */
    int info;

    tids = new int[nproc];
    assert (tids != NULL);
    
        /* enroll in pvm */
    mytid = pvm_mytid();
    if (mytid<0)
        my_print_error("vnus_exec_forkall",mytid);
    
        /* Join a group and if I am the first instance */
        /* i.e. me=0 spawn more copies of myself       */
    
    me = pvm_joingroup( const_cast<char *>(my_pvm_group) );
    if (me<0)
        my_print_error("vnus_exec_forkall",mytid);
#ifdef TRACE_ON
    fprintf(stderr, "me = %d mytid = %d\n",me,mytid);
#endif

    if( (me == 0) && (nproc > 1) )
    {
#ifdef TRACE_ON
        fprintf(stderr,"v_e_f:0\n");
#endif

#ifdef __PVM_ROUND_ROBIN__
        int nhost, narch;
        struct pvmhostinfo *hostp;
        info = pvm_config(&nhost, &narch, &hostp);
        if (info < 0)
            my_print_error("Could not do pvm_config",mytid);
#ifdef TRACE_ON
        printf("nhost:%d, narch:%d\n",nhost,narch);
        for (int j=0; j<nhost; j++)
            printf("Host %d = %s\n",j,hostp[j].hi_name);
#endif
        for (int j=1; j<nproc; j++)
        {
            info = pvm_spawn(argv[0], (char**)0, 1, hostp[j%nhost].hi_name, 1, &tids[j]);
            if (info < 0)
                my_print_error("vnus_exec_forkall", info);
        }
        
#else
        info = pvm_spawn(argv[0], (char**)0, 0, "", nproc-1, &tids[1]);
        if (info < (nproc-1))
        {
            if (info >= 0)
            {
                fprintf(stderr,"spawned %s %d times, expected %d\n",argv[0], info,nproc-1);
                for (int i=1; i<nproc; i++)
                    fprintf(stderr,"pos %d:err %d\n",i,tids[i]);
                my_print_error("vnus_exec_forkall", tids[1+info]);
            }
            else
            {   
                my_print_error("vnus_exec_forkall", info);
            }
        }
#endif
    }
    
#ifdef TRACE_ON
    fprintf(stderr,"v_e_f:1\n");
#endif
        /* Wait for everyone to startup before proceeding. */
    info = pvm_freezegroup(my_pvm_group, nproc);
    if (info<0)
        my_print_error("vnus_exec_forkall",info);
#ifdef TRACE_ON
    fprintf(stderr,"v_e_f:2\n");
#endif
    info = pvm_barrier( const_cast<char *>(my_pvm_group), nproc );
    if (info<0)
        my_print_error("vnus_exec_forkall",info);
    
#ifdef TRACE_ON
    fprintf(stderr,"v_e_f:3\n");
#endif
    hiddenThisProcessor = me;
    body(me, argc, argv);

        // Extra barrier so that all processes terminate together
    info = pvm_barrier( const_cast<char *>(my_pvm_group), nproc );
    if (info<0)
        my_print_error("vnus_exec_forkall",info);

        /* program finished leave group and exit pvm */
#ifdef TRACE_ON
    fprintf(stderr,"v_e_f:4\n");
#endif
    info = pvm_lvgroup( const_cast<char *>(my_pvm_group) );
    if (info<0)
        my_print_error("vnus_exec_forkall",info);
#ifdef TRACE_ON
    fprintf(stderr,"v_e_f:5\n");
#endif
    info = pvm_exit();
    if (info<0)
        my_print_error("vnus_exec_forkall",info);
#ifdef TRACE_ON
    fprintf(stderr,"v_e_f:6\n");
#endif
    delete [] tids;
    exit(0);
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
