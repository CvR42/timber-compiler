/* $Id: GLBClusterItem.C,v 1.1 2000/10/19 21:09:45 andi Exp $ */

#include <stdio.h>

#include "../data/Sched.H"
#include "../data/HeapItem.H"
#include "GLBClusterItem.H"


GLBClusterItem::GLBClusterItem (int v_idx, int v_work)
{
  idx = v_idx;
  work = v_work;
  ST = NONE;
}


bool
GLBClusterItem::LessPrioThan (PHeapItem item) const
{
  PGLBClusterItem the_other = (PGLBClusterItem) item;

  if (ST > the_other->ST)
    return true;
  else if (ST == the_other->ST)
    if (work < the_other->work)
      return true;

  return false;
}

bool
GLBClusterItem::MorePrioThan (PHeapItem item) const
{
  PGLBClusterItem the_other = (PGLBClusterItem) item;

  if (ST < the_other->ST)
    return true;
  else if (ST == the_other->ST)
    if (work > the_other->work)
      return true;

  return false;
}


int
GLBClusterItem::GetIDX (void) const
{
  return idx;
}

void
GLBClusterItem::SetST (sched_time val)
{
  ST = val;
}

sched_time
GLBClusterItem::GetST (void) const
{
  return ST;
}


void
GLBClusterItem::SetWork (sched_flops val)
{
  work = val;
}

void
GLBClusterItem::AddWork (sched_flops val)
{
  work += val;
}

sched_flops
GLBClusterItem::GetWork (void) const
{
  return work;
}


void
GLBClusterItem::SetDestProc (int pid)
{
  dest_pid = pid;
}

int
GLBClusterItem::GetDestProc (void) const
{
  return dest_pid;
}


void
GLBClusterItem::Print (void) const
{
  printf ("%2d(%24.1f,%2ld)", idx, ST, work);
}

void
GLBClusterItem::PrintVoid (void) const
{
  printf ("      ");
}
