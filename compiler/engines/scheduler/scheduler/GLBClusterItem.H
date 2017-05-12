/* $Id: GLBClusterItem.H,v 1.1 2000/10/19 21:09:45 andi Exp $ */

#ifndef __GLBCLUSTERITEM_H
#define __GLBCLUSTERITEM_H

#include "../data/Sched.H"
#include "../data/HeapItem.H"
#include "GLBNode.H"


typedef class GLBClusterItem *PGLBClusterItem;

class GLBClusterItem
  : public HeapItem
{
  public:
    GLBClusterItem (int idx, int work = 0);

    bool LessPrioThan (PHeapItem item) const;
    bool MorePrioThan (PHeapItem item) const;
    int GetIDX (void) const;

    void SetST (sched_time val);
    sched_time GetST (void) const;
    void SetWork (sched_flops val);
    void AddWork (sched_flops val);
    sched_flops GetWork (void) const;

    void SetDestProc (int pid);
    int GetDestProc (void) const;
    void Print (void) const;
    void PrintVoid (void) const;

  private:
    int idx;
    sched_time ST;
    sched_flops work;

    int dest_pid;
};

#endif
