/* File: ptclass.h
 *
 * Wrapper classes around the pthreads package.
 */

#ifndef __PTCLASS_H__
#define __PTCLASS_H__

#//ifdef __linux__
#//define _MIT_POSIX_THREADS 1
#//endif
#include <pthread.h>
#include <stdio.h>

#include <assert.h>
#include <stdlib.h>

#include "vnustypes.h"
#include "vnusrtl.h"

#if 0
#if _MIT_POSIX_THREADS
#include <pthread/mit/posix.h>
#else
#include <pthread/posix.h>
#endif
#endif

#define OK 0

class PTThread;
class PTCondition;
class PTMutex;

/*
 * Spinlock
 * can be locked and unlocked
 */
class PTMutex
{
  private:
    pthread_mutex_t theLock;
  public:
    PTMutex();
    ~PTMutex();
    void lock();
    void unlock();
    friend class PTCondition;
};

/*
 * Condition variable
 * can be used for waiting, signalling and broadcasting
 * waiting requires a PTMutex
 */
class PTCondition
{
  private:
    pthread_cond_t theSignal;
  public:
    PTCondition();
    ~PTCondition();
    void wait(PTMutex &lock);
    void signal();
    void broadcast();
};

/*
 * Simple thread starting mechanism
 * can be used on its own through the parameterized constructor, or as
 * base class via the default constructor and the start(0 function
 */
class PTThread
{
  private:
    pthread_t tid;
  public:
    PTThread(void* (*func)(void*), void *data);
    PTThread(){};
    void start(void* (*func)(void*), void *data);
    inline VnusBoolean isThisMe();
};

inline VnusBoolean
PTThread::isThisMe()
{
    return pthread_equal(pthread_self(), tid);
}

#endif
