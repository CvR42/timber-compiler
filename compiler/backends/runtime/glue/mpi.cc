/* File: mpi.cc
 *
 * Service functions for the pvm version of the backend.
 */

#include <stdio.h>

#include <assert.h>
#include <stdlib.h>
#include <assert.h>

#include "vnusstd.h"
#include "vnusrtl.h"
#include "vnusprocdefs.h"
#include "shape.h"

#include <mpi.h>

#include "partsend.h"
#include "mpihack.h"

//#define TRACE_ON

#define TAG_ANY -1
#define TAG_HEADER 1
#define TAG_MSG 2
#define TAG_RETURN 3
#define GIVE_LOTS_OF_SPACE (2*1024*1024*4) // Enough for lots of messages
//#define UNKNOWN_BUF_SIZE (128*1024*8*4) // Enough for 1 message

static VnusInt hiddenThisProcessor;
static MPI_Group OurGroup;
static MPI_Comm OurComm;

static void MPI_CHK_ERR(int err, const char *s)
{
    if (err != 0)
        fprintf(stderr, "mpi.cc: error %d in %s\n",err, s);
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
    MPI_CHK_ERR(MPI_Barrier(OurComm),"MPI_Barrier");
}

/* Given a destination processor, the number
 * of bytes to send, and a pointer to a
 * buffer, send the elements to the given processor.
 *
 * Any problems are reported within this function, so there is
 * no result.
 */
void vnus_send_real(const int dest, const int size, const void *buf )
{
#ifdef TRACE_ON
    fprintf(stderr,"v_snd_real:1\n");
#endif
    if (dest == thisProcessor())
    {
	Hack_Bsend((const void *)buf, size);
    }
    else
    {
	MPI_CHK_ERR(MPI_Bsend((void *)buf, size, MPI_BYTE, dest, TAG_MSG, OurComm),"MPI_Bsend");
    }
}

/* Given a source processor, bytes to be received,
 * and a pointer to a
 * buffer, receive the elements from the given processor.
 *
 * If bytes received doesn't match requested size an error is erported.
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
    MPI_Status status;
    int count;

    if (src != thisProcessor())
    {
        MPI_CHK_ERR(MPI_Recv(buf, size, MPI_BYTE, src, TAG_MSG,
                 OurComm, &status),"MPI_Recv");
        MPI_CHK_ERR(MPI_Get_count(&status,MPI_BYTE,&count),"MPI_Get_count");
    }
    else
    {
        Hack_Recv(buf, &count);
    }
    if (size != count)
	runtime_error("MPI::vnus_receive::recv'ed size doesn't match");
//        *nelm = status.st_length/elmsz;
}

void vnus_broadcast( const int size, const void *buf )
{
    for (int i=0; i<numberOfProcessors; i++)
      vnus_send_real(i,size,buf);
}

void vnus_exec_forkall( void (*body)( int p, int, char** ), int nproc, int argc, char** argv )
{
    int myrank;                  /* my task id */
    int noTasks;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
    hiddenThisProcessor = myrank;
    MPI_Comm_size(MPI_COMM_WORLD, &noTasks);
    if (myrank == 0) {
      if (noTasks < nproc) {
	fprintf(stderr,"ERROR : Actual nr. of processes(%d) < specified nr.(%d)!\n", noTasks, nproc);
      } else if (noTasks > nproc) {
	fprintf(stderr,"WARNING : Actual nr. of processes(%d) > specified nr.(%d)!\n", noTasks, nproc);
      }
    }
    if (noTasks < nproc) {
        MPI_Finalize();
        exit(1);
    }
    if (myrank < nproc) {// YUCK!!!
        int *theRanks = new int[nproc];
        assert(theRanks != NULL);
        for (int i=0; i<nproc; i++)
            theRanks[i] = i;
        MPI_Group WorldGroup;
        MPI_Comm_group(MPI_COMM_WORLD, &WorldGroup);
        MPI_Group_incl(WorldGroup, nproc, theRanks, &OurGroup);
        MPI_Comm_create(MPI_COMM_WORLD, OurGroup, &OurComm);
	//        unsigned char *aBuffer = new unsigned char[GIVE_LOTS_OF_SPACE];
	unsigned char *aBuffer = (unsigned char *)malloc(GIVE_LOTS_OF_SPACE);
        assert(aBuffer != NULL);
        MPI_Buffer_attach(aBuffer, GIVE_LOTS_OF_SPACE);
        body(myrank,argc,argv);
        {
            unsigned char *buf = aBuffer;
            int bufsize = GIVE_LOTS_OF_SPACE;
            MPI_Buffer_detach(&buf, &bufsize);
        }
        delete [] theRanks;
	//        delete [] aBuffer;
	free(aBuffer);
    }
    MPI_Finalize();
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
