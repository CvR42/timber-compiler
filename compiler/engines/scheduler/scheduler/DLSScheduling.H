/* $Id: DLSScheduling.H,v 1.1 2000/10/19 21:09:44 andi Exp $ */

#ifndef _A_DLSSCHEDULING_H
#define _A_DLSSCHEDULING_H

#include "../data/List.H"
#include "Scheduling.H"
#include "DSA_Node.H"


class DLSScheduling
  : public Scheduling
{
  public:
     /** Build the ready task list. */
    DLSScheduling (void);
    ~DLSScheduling ();

    void Schedule (void);

  private:
    PList ready_tasks_list;

    void ScheduleTask (void);

    void UpdateReadyTaskList (DSA_Node *node);
};

#endif
