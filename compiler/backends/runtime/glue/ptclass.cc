/* File: vnusthread.cc
 *
 * Service functions for the thread version of the backend.
 */

#include "ptclass.h"
/*
 * Wrapper classes around the pthreads package
 */

static void PT_error( const char *context, const int errNo=0)
{
    if (errNo==0)
        fprintf( stderr, "%s::%s error:%d\n", __FILE__, context, errNo );
    else
        fprintf( stderr, "%s::%s\n", __FILE__, context );
    exit(1);
}


/*
 * Spinlock
 * can be locked and unlocked
 */
PTMutex::PTMutex()
{
    int err = pthread_mutex_init(&theLock, NULL);
    if (err != OK) PT_error("PTMutex()", err);
}

PTMutex::~PTMutex()
{
    int err = pthread_mutex_destroy(&theLock);
    if (err != OK) PT_error("~PTMutex()", err);
}

void
PTMutex::lock()
{
    int err = pthread_mutex_lock(&theLock);
    if (err != OK) PT_error("PTMutex::lock()", err);
}

void
PTMutex::unlock()
{
    int err = pthread_mutex_unlock(&theLock);
    if (err != OK) PT_error("PTMutex::unlock()", err);
}


/*
 * Condition variable
 * can be used for waiting, signalling and broadcasting
 * waiting requires a PTMutex
 */
PTCondition::PTCondition()
{
    int err = pthread_cond_init(&theSignal, NULL);
    if (err != OK) PT_error("PTCondition()", err);
}

PTCondition::~PTCondition()
{
    int err = pthread_cond_destroy(&theSignal);
    if (err != OK) PT_error("~PTCondition()", err);
}

void
PTCondition::wait(PTMutex &lock)
{
    int err = pthread_cond_wait(&theSignal, &lock.theLock);
    if (err != OK) PT_error("PTCondition::wait()", err);
}

void
PTCondition::signal()
{
    int err = pthread_cond_signal(&theSignal);
    if (err != OK) PT_error("PTCondition::signal()", err);
}

void
PTCondition::broadcast()
{
    int err = pthread_cond_broadcast(&theSignal);
    if (err != OK) PT_error("PTCondition::broadcast()", err);
}


/*
 * Simple thread starting mechanism
 * can be used on its own through the parameterized constructor, or as
 * base class via the default constructor and the start() function
 */
PTThread::PTThread(void* (*func)(void*), void *data)
{
    start(func, data);
}

void
PTThread::start(void* (*func)(void*), void *data)
{
    int err = pthread_create(&tid, NULL, func, data);
    if (err != OK) PT_error("start()", err);
}
