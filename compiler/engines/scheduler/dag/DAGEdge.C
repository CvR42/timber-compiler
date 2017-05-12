/* $Id: DAGEdge.C,v 1.1 2000/10/19 21:09:22 andi Exp $ */

#include "../data/Sched.H"
#include "../data/arguments.h"
#include "../data/Node.H"
#include "../processors/Processors.H"
#include "../scheduler/Scheduling.H"
#include "DAGEdge.H"


#define MYRINET_LATENCY_SIZE_LIMIT  256       // bytes
/*
#define MYRINET_MIN_LATENCY          96       // microsec
#define MYRINET_THROUGHPUT           12       // Mbits/sec
*/

#define MYRINET_MIN_LATENCY          100       // microsec
#define MYRINET_THROUGHPUT            10       // Mbits/sec

#define ETHERNET_MIN_LATENCY       1000       // microsec
#define ETHERNET_THROUGHPUT           0.1     // Mbits/sec



DAGEdge::DAGEdge (int v_eid, PNode from_node, PNode to_node,
                  sched_msg_size v_msg_size)
    : Edge (from_node, to_node)
{
  eid = v_eid;
  msg_size = v_msg_size;
}

  
int
DAGEdge::GetID (void) const
{
  return eid;
}

  
sched_msg_size
DAGEdge::GetMsgSize (void) const
{
  return msg_size;
}


   // The result is in microseconds
   // size in bytes, latency in microsec, throughput in Mbytes/sec
sched_time
DAGEdge::GetCommTime (int src_pid, int dest_pid) const
{
  if ((src_pid == dest_pid) && (src_pid != NONE))
    return 0;

  switch (NETWORK_TYPE)
  {
    case MYRINET:
      return MYRINET_MIN_LATENCY + msg_size / MYRINET_THROUGHPUT;
    case ETHERNET:
      return ETHERNET_MIN_LATENCY + msg_size / ETHERNET_THROUGHPUT; 
    case EXEC_TIME:
      return msg_size;
  }

  return -1;
}

   // The result is in miliseconds
   // size in bytes, latency in microsec, throughput in Mbytes/sec
sched_time
DAGEdge::GetCommTime (int network_type) const
{
  switch (network_type)
  {
    case MYRINET:
      return MYRINET_MIN_LATENCY + msg_size / MYRINET_THROUGHPUT;
    case ETHERNET:
      return ETHERNET_MIN_LATENCY + msg_size / ETHERNET_THROUGHPUT;
    case EXEC_TIME:
      return msg_size;
  }

  return -1;
}

   // The result is in miliseconds
   // size in bytes, latency in microsec, throughput in Mbytes/sec
sched_time
DAGEdge::GetCommTime (void) const
{
  switch (NETWORK_TYPE)
  {
    case MYRINET:
      return MYRINET_MIN_LATENCY + msg_size / MYRINET_THROUGHPUT;
    case ETHERNET:
      return ETHERNET_MIN_LATENCY + msg_size / ETHERNET_THROUGHPUT;
    case EXEC_TIME:
      return msg_size;
  }

  return -1;
}
