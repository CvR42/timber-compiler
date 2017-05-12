#ifndef __TASKPAR_H__
#define __TASKPAR_H__

#include "vnustypes.h"
#include "shaped1.h"

class base_record {
public:
};

// All task_id_records start as follows:task
class task_id_record_header: public base_record
{
  public:
    VnusInt tir_size; // sizeof current record
    VnusInt task_id;
	// indices...
};

// All parameter records start as follows:
class parameter_record_header: public base_record
{
  public:
    VnusInt pr_size; // sizeof current record
	// parameters...
};

/*
  All void pointers should actually be pointers to above classes.
  Unfortunately, vnus doesn't provide polymorphism, so we have to do
  it this way, with casts inside the functions.
  */

/*
  Determine the processor a task_id_record is bound to.
  */
extern int get_task_processor(void *tir);
extern int get_task_priority(void *tir);
extern void add_task(void *tir, void *pr);
extern void register_task(int id, void (*f)(void*, void*));
extern void execute_tasks();

/*
  All messages are composed of a task_id_record and a
  parameter_record. Both are variable length records.
  */
extern void tsend(void *tir, int size, void *buf);
extern void treceive(void *tir, int size, void *buf);

/*
  The processor and priority lists have to be known to the runtime system.
  */
extern void register_processor_list(ShapeD1<VnusInt> *proc_l);
extern void register_priority_list(ShapeD1<VnusInt> *prio_l);

#endif
