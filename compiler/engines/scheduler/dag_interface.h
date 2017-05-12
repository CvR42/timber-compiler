/* File: dag_interface.h */

#ifndef _a_DAG_INTERFACE_H
#define _a_DAG_INTERFACE_H

#include <stdio.h>

#include "data/Sched.H"


int add_task_to_dag (char* name, int nr_card, unsigned int* card, int flops);
void add_depend_to_dag (unsigned int from_task_id, unsigned int to_task_id,
                        int msg_size);

#endif
