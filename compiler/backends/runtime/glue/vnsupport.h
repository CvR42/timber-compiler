/*
 * Vnus primitives
 */

#ifndef __VNSUPPORT_H__
#define __VNSUPPORT_H__

#include "ptclass.h"

class VnusBarrier;
class VnusSemaphore;
class VnusMessage;
class VnusPort;
class VnusThread;
class VnusThreadBase;

/*
 * barrier
 * the last process that wants to 'meet' at the barrier will cause the
 * others to resume
 */
class VnusBarrier
{
  private:
    PTMutex theLock;
    PTCondition theSignal;
    int max;
    int arrivals;
  public:
    VnusBarrier(int n) : max(n), arrivals(0) {}
    void meet();
};

/*
 * counting semaphore
 * a process will 'wait' for the semaphore if there are no 'signal's present,
 * otherwise it will continue
 * if  'signal' arrives a 'waiting' process will be awakened
 */
class VnusSemaphore
{
  private:
    PTMutex theLock;
    PTCondition theSignal;
    int waiting;
  public:
    VnusSemaphore() : waiting(0) {}
    void signal();
    void wait();
};

class VnusMessage
{
  public:
    VnusMessage *next;
    int nElm;
    int messSize;
    char* theBuf;
    VnusMessage(const int size, const void *buf);
    ~VnusMessage();
};

/*
 * A mailbox system intended for use with threads
 * A mailbox will store multiple messages, and it will process messages
 * asynchronously. The sender can continue after having delivered
 * the message to this mailbox.
 */
class VnusPort
{
  private:
    PTMutex theLock;
    VnusMessage *head, *tail;
    VnusSemaphore haveSome;
//    VnusSemaphore gotIt;
  public:
    VnusPort();
    void Send(const int size, const void *buf);
    void Recv(const int size, void *buf);
};

/*
 * Base that spawns threads...
 */
class VnusThreadBase
{
  public:
    VnusBarrier *theBarrier;
    VnusBarrier *finalBarrier;
    VnusThread **theThreads;
    void (*body)(int, int, char**);
    int nProc;
    int argC;
    char **argV;
    VnusThreadBase( void (*body)( int p, int, char** ), int nproc, int argc, char **argv );
    void start();
    ~VnusThreadBase();
    VnusInt getThisProcessor();
    VnusBoolean isThisProcessor(const VnusInt procNo);
};

/*
 * higher level thread
 * Each thread contains one incoming mailbox for each other thread
 */
class VnusThread : private PTThread
{
  public:
    VnusThreadBase *myParent;
//    PTThread *theThread;
    void (*body)(int,int,char**);
    int pid;
    int argC;
    char **argV;
    VnusPort *msgPort;
    VnusThread(VnusThreadBase *parent, void (*func)(int, int, char**), int p, int nproc, int argc, char **argv);
    void start();
    ~VnusThread();
    inline VnusBoolean isThisMe() { return PTThread::isThisMe(); };
};

#endif
