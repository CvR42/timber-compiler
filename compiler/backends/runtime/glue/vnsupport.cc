#include "vnsupport.h"

#define OK 0

//#define DEBUG_VnusBarrier
//#define DEBUG_VnusCommunication

static void VNS_error( const char *context, const int errNo=0)
{
    if (errNo!=0)
        fprintf( stderr, "%s::%s; error:%d\n", __FILE__, context, errNo );
    else
        fprintf( stderr, "%s::%s\n", __FILE__, context );
    exit(1);
}


/*
 * Vnus primitives
 */


/*
 * barrier
 * the last process that wants to 'meet' at the barrier will cause the
 * others to resume
 */
void VnusBarrier::meet()
{
    theLock.lock();
    arrivals++;
    if (arrivals == max)
    {
        arrivals = 0;
        theLock.unlock();
#ifdef DEBUG_VnusBarrier
        fprintf(stderr, "VnusBarrier: %d of %d -> broadcast %p\n", max, max, this);
#endif
        theSignal.broadcast();
    }
    else
    {
#ifdef DEBUG_VnusBarrier
        fprintf(stderr, "VnusBarrier: %d of %d -> wait for %p\n", arrivals, max, this);
#endif
        theSignal.wait(theLock);
        theLock.unlock();
    }
}


/*
 * counting semaphore
 * a process will 'wait' for the semaphore if there are no 'signal's present,
 * otherwise it will continue
 * if  'signal' arrives a 'waiting' process will be awakened
 */
void
VnusSemaphore::signal()
{
    theLock.lock();
    waiting--;
    if (waiting >= 0)
    {
        theLock.unlock();
        theSignal.signal();
    }
    else
    {
        theLock.unlock();
    }
}

void
VnusSemaphore::wait()
{
    theLock.lock();
    waiting++;
    if (waiting <= 0)
    {
        theLock.unlock();
    }
    else
    {
        theSignal.wait(theLock);
        theLock.unlock();
    }
}


VnusMessage::VnusMessage(const int size, const void *buf) 
 : next(NULL), messSize(size)
{
    theBuf = new char[messSize];
    if (theBuf == NULL)
        VNS_error("VnusMessage::VnusMessage(): new char[] failed");
    memcpy(theBuf, buf, messSize);
}

VnusMessage::~VnusMessage()
{
    delete [] theBuf;
}


/*
 * a mailbox system intended for use with threads
 * A mailbox will only store one message, and it will process messages
 * synchronously. The sender will continue after the recipient has processed
 * the message.
 *
 * Whoops, old comment: now multiple messages and async! Joepie!
 */
VnusPort::VnusPort()
{
    head = NULL;
    tail = NULL;
}

void
VnusPort::Send(const int size, const void *buf)
{
    VnusMessage *msg = new VnusMessage(size, buf);
    theLock.lock();
    if (tail != NULL)
        tail->next = msg;
    tail = msg;
    if (head == NULL)
        head = tail;
    theLock.unlock();
    haveSome.signal();
//    gotIt.wait();
}

void
VnusPort::Recv(const int size, void *buf)
{
    haveSome.wait();
    theLock.lock();
    VnusMessage *msg = head;
    head = head->next;
    if (head == NULL)
        tail = head;
    theLock.unlock();
    if (size != msg->messSize)
    {
        fprintf(stderr, "Recv: expected size:%d unequal to received size:%d\n",
                size, msg->messSize);
        exit(1);
    }
    memcpy(buf, msg->theBuf, msg->messSize);
    delete msg;
//    gotIt.signal();
}


/*
 * Base that spawns threads...
 */
VnusThreadBase::VnusThreadBase(void (*func)(int, int, char**), int nproc, int argc, char **argv)
 : body(func), nProc(nproc), argC(argc), argV(argv)
{
    theBarrier = new VnusBarrier(nProc);
    if (theBarrier == NULL)
        VNS_error("VnusThreadBase::VnusThreadBase(): new VnusBarrier[] failed");
    finalBarrier = new VnusBarrier(nProc+1);
    if (finalBarrier == NULL)
        VNS_error("VnusThreadBase::VnusThreadBase(): new VnusBarrier[] failed");
    theThreads = new VnusThread*[nProc];
    if (theThreads == NULL)
        VNS_error("VnusThreadBase::VnusThreadBase(): new VnusThread*[] failed");
    for (int p=0; p<nProc; p++)
    {
        theThreads[p] = new VnusThread(this, body, p, nProc, argC, argV);
        if (theThreads == NULL)
            VNS_error("VnusThreadBase::VnusThreadBase(): new VnusThread[] failed");
    }
}

/*
 * We have to use a separate start() function as it is unsafe to start threads
 * before the whole environment is set up.
 */
void
VnusThreadBase::start()
{
    for (int p=0; p<nProc; p++)
    {
        theThreads[p]->start();
    }
    finalBarrier->meet();
}

VnusThreadBase::~VnusThreadBase()
{
    for (int p=0; p<nProc; p++)
    {
        delete theThreads[p];
    }
    delete [] theThreads;
    delete finalBarrier;
    delete theBarrier;
}

VnusInt VnusThreadBase::getThisProcessor()
{
    for (int p=0; p<nProc; p++)
    {
        if (theThreads[p]->isThisMe())
            return p;
    }
    VNS_error("VnusThreadBase(): can't find this thread");
    return 0;
}

VnusBoolean VnusThreadBase::isThisProcessor(const VnusInt procNo)
{
    return (theThreads[procNo]->isThisMe());
}


static void wrapper(void*);

/*
 * higher level thread
 * Each thread contains one incoming mailbox for each other thread
 */
VnusThread::VnusThread(VnusThreadBase *parent, void (*func)(int,int,char**), int p, int nproc, int argc, char **argv)
 : myParent(parent), body(func), pid(p), argC(argc), argV(argv)
{
    msgPort = new VnusPort[nproc];
    if (msgPort == NULL)
        VNS_error("VnusThread(): new VnusPort[] failed");
}

void
VnusThread::start()
{
    PTThread::start((void* (*)(void*))wrapper, this);
}

VnusThread::~VnusThread()
{
    delete [] msgPort;
}

static void wrapper(void *data)
{
    VnusThread *tPtr = (VnusThread*) data;
    tPtr->body(tPtr->pid,tPtr->argC,tPtr->argV);
    tPtr->myParent->finalBarrier->meet();
}
