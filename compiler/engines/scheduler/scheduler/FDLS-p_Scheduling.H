/* $Id: FDLS-p_Scheduling.H,v 1.1 2000/10/19 21:09:45 andi Exp $ */

#ifndef _A_FDLS_p_SCHEDULING_H
#define _A_FDLS_p_SCHEDULING_H

#include "../data/Heap.H"
#include "../data/PseudoHeap.H"
#include "../data/DirectHeap.H"
#include "../data/PseudoHeapItem.H"
#include "../data/PrioItem.H"
#include "Scheduling.H"
#include "FDLS-p_Node.H"


/**
  FDLS-f  by  Andrei - Jul 2000

  Characteristics
    - equivalent to O(W(E+V)P)'s DLS by Sih and Lee
    - bounded nr. of processors
    - no duplication
    - O (V log(W) + E)
    
  Algorithm description
    prio (t,p) = BL(t) - max (EMT(t,p), PRT(p))

               = BL(t) - max (LMT(t), PRT(p)),     if p != EP(t)
                 BL(t) - max (EMT(t,p), PRT(p)),   if p = EP(t)

    The following values are used:
      LMT(t) = last message arrival time (no comm cost zeroing).
      EMT(t,p) = minimum start time, obtained by zeroing 
                 the communication costs from the P(t)
                 from which the last message arrives.
      PRT(p) = precessor ready time.
      EP(t) = t's enabeling processor (i.e., the processor t's last message 
              is sent from)

    - When the task t becomes ready, LMT(t), EMT(t,EP(t)) are computed.
    - EP tasks:
      - decreasingly sorted by EMT(t,EP(t)) - BL(t) on their enabeling 
        processors.
      - top EP tasks on each processor are decreasingly sorted by prio(t,p).
    - non-EP tasks:
      - decreasingly sorted by LMT(t).
    - processors:
      - decreasingly sorted by PRT(p).

    - The two pair candidates:
      - EP task and its enabeing processor with minimum prio(t,p).
      - non-EP task with minimum LMT(t) and processor with minimum PRT(p).
    - The pair with the highest prio(t,p) is scheduled.
*/
class FDLS_p_Scheduling
  : public Scheduling
{
  public:
     /// Build the task and processor priority lists.
    FDLS_p_Scheduling (double pseudo_heap_size);
    ~FDLS_p_Scheduling ();

     /// While there are unscheduled tasks, schedule one.
    void Schedule (void);

  private:
     /// Processor lists, sorted by the time the processors become idle. 
    PDirectHeap proc_list;

     /// Ready non-EP tasks, sorted by BL(t).
    PPseudoHeap BL_nonEP_task_list;
    PPseudoPrioItem *BL_nonEP_task_item;
     /// Ready non-EP tasks, sorted by BL-LMT(t).
    PPseudoHeap prio_nonEP_task_list;

     /// Ready EP tasks enabled by the list's processor, sorted by EMT(t,EP(t))
    PHeap *proc_EP_task_list;
    PPrioItem *proc_EP_task_item;

     /// Processor, decreasingly sorted by their prio with their top task
     /// prio(t,p) = BL(t) - max (EMT(t,p), PRT(p))
    PDirectHeap active_proc_list;


    PFDLS_p_Node ScheduleTask (void);
};

#endif
