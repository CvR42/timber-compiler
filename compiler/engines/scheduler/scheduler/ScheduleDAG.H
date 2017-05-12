/* $Id: ScheduleDAG.H,v 1.1 2000/10/19 21:09:47 andi Exp $ */

#ifndef __SCHEDULE_DAG_H
#define __SCHEDULE_DAG_H

#include <sys/time.h>
#include <unistd.h>


void ScheduleDAG (void);
void PrintSchedule (timeval *delay);
void Simulate (void);

#endif

