
#define EMB_USE_TIMING
#include "ensemble.h"
#include <stdio.h>

#define Z 30
#define SHOWFENCE 0

#define SENDER 1
#define RECEIVER 0

#define TESTUU 1
#define TESTBU 0
#define TESTUB 0
#define TESTBB 0

typedef int Array[Z][Z];

/*   / 0 0 0 0 0 0 0 0 0 0 \    / 0 1 2 3 0 1 2 3 0 1 \
 *   | 1 1 1 1 1 1 1 1 1 1 |    | 0 1 2 3 0 1 2 3 0 1 |
 *   | 2 2 2 2 2 2 2 2 2 2 |    | 0 1 2 3 0 1 2 3 0 1 |
 *   | 3 3 3 3 3 3 3 3 3 3 |    | 0 1 2 3 0 1 2 3 0 1 |
 *   | 0 0 0 0 0 0 0 0 0 0 | -> | 0 1 2 3 0 1 2 3 0 1 |
 *   | 1 1 1 1 1 1 1 1 1 1 |    | 0 1 2 3 0 1 2 3 0 1 |
 *   | 2 2 2 2 2 2 2 2 2 2 |    | 0 1 2 3 0 1 2 3 0 1 |
 *   | 3 3 3 3 3 3 3 3 3 3 |    | 0 1 2 3 0 1 2 3 0 1 |
 *   | 0 0 0 0 0 0 0 0 0 0 |    | 0 1 2 3 0 1 2 3 0 1 |
 *   \ 1 1 1 1 1 1 1 1 1 1 /    \ 0 1 2 3 0 1 2 3 0 1 /
 */

int main(int argc, char **argv)

{
  int p,np;

  emb_init(argc, argv, 2, 1);

  p  = emb_PID();
  np = emb_NP();

  if (p>=0)
    {
      Array A,B;
      TimerValue T1,T2;

      int i,j,repeat;

      for(i=0;i<Z;i++)
	for(j=0;j<Z;j++)
	  A[i][j]=100*(p)+(i+1)*10+(j+1);
      
      for(i=0;i<Z;i++)
	for(j=0;j<Z;j++)
	  B[i][j]=0xdeadbeef;

      for(repeat=1;repeat<=100;repeat++)
	{

	  if (TESTUU)
	    { /* U -> U */
	      int z;

	      /* prepare SENDS */
	      
	      if (p==SENDER)
		{
		  int send_strides[2],send_stride_counts[2];

		  send_strides[0]       = 1; send_stride_counts[0] = Z;
		  send_strides[1]       = Z; send_stride_counts[1] = Z;
		  
		  for(z=1;z<=5;z++)
		    {
		      emb_send(1-p,sizeof(int),A,0,2,send_strides, send_stride_counts,0);
		    }
		}
	      
	      /* prepare RECEIVES */
	      
	      
	      if (p==RECEIVER)
		{
		  int recv_strides[2],recv_stride_counts[2];
		  
		  recv_strides[0]       = 1; recv_stride_counts[0] = Z;
		  recv_strides[1]       = Z; recv_stride_counts[1] = Z;

		  for(z=1;z<=5;z++)
		    emb_recv(1-p,sizeof(int),B,0,2,recv_strides, recv_stride_counts,0);
		}
	      
	      /* perform SENDS and RECEIVES */
	      
	      T1 = timer_value();
	      emb_fence();
	      T2 = timer_value();
	      
	      if (SHOWFENCE)
		{
		  int N    = 4*Z*Z;
		  double T = timer_double(T2-T1);
		  
		  printf("[%d] --> fence U->U #%06d: %d bytes in %10.5f seconds [%10.3f MB/sec].\n",
			 p,repeat,N,T,(N/1048576.0)/T);
		}
	    }

	  if (TESTBU)
	    { /* B -> U */

	      if (p==SENDER)
		{
		  int send_strides[3],send_stride_counts[3];
		  
		  send_strides[0]       = 2; send_stride_counts[0] = Z/2;
		  send_strides[1]       = 1; send_stride_counts[1] = 2;
		  send_strides[2]       = Z; send_stride_counts[2] = Z;
		  
		  emb_send(1-p,sizeof(int),A,0,3,send_strides, send_stride_counts,0);
		}
	      
	      /* prepare RECEIVES */
	      

	      if (p==RECEIVER)
		{
		  int recv_strides[2],recv_stride_counts[2];
		  
		  recv_strides[0]       = 1; recv_stride_counts[0] = Z;
		  recv_strides[1]       = Z; recv_stride_counts[1] = Z;
		  
		  emb_recv(1-p,sizeof(int),B,0,2,recv_strides, recv_stride_counts,0);
		}
	      
	      /* perform SENDS and RECEIVES */
	      
	      emb_barrier();
	      T1 = timer_value();
	      emb_fence();
	      emb_barrier();
	      T2 = timer_value();
	      
	      if (SHOWFENCE)
		{
		  int N    = 4*Z*Z;
		  double T = timer_double(T2-T1);
		  
		  printf("[%d] --> fence B->U #%06d: %d bytes in %10.5f seconds [%10.3f MB/sec].\n",
			 p,repeat,N,T,(N/1048576.0)/T);
		}
	    }

	  if (TESTUB)
	    { /* U -> B */

	      if (p==SENDER)
		{
		  int send_strides[2],send_stride_counts[2];
		  
		  send_strides[0]       = 1; send_stride_counts[0] = Z;
		  send_strides[1]       = Z; send_stride_counts[1] = Z;
		  
		  emb_send(1-p,sizeof(int),A,0,2,send_strides, send_stride_counts,0);
		}
	  
	      /* prepare RECEIVES */
	      

	      if (p==RECEIVER)
		{
		  int recv_strides[3],recv_stride_counts[3];
		  
		  recv_strides[0]       = 2; recv_stride_counts[0] = Z/2;
		  recv_strides[1]       = 1; recv_stride_counts[1] = 2;
		  recv_strides[2]       = Z; recv_stride_counts[2] = Z;
		  
		  emb_recv(1-p,sizeof(int),B,0,3,recv_strides, recv_stride_counts,0);
		}
	      
	      /* perform SENDS and RECEIVES */
	      
	      emb_barrier();
	      T1 = timer_value();
	      emb_fence();
	      emb_barrier();
	      T2 = timer_value();
	      
	      if (SHOWFENCE)
		{
		  int N    = 4*Z*Z;
		  double T = timer_double(T2-T1);
		  
		  printf("[%d] --> fence U->B #%06d: %d bytes in %10.5f seconds [%10.3f MB/sec].\n",
			 p,repeat,N,T,(N/1048576.0)/T);
		}
	    }

	  if (TESTBB)
	    { /* B -> B */

	      if (p==SENDER)
		{
		  int send_strides[3],send_stride_counts[3];
		  
		  send_strides[0]       = 2; send_stride_counts[0] = Z/2;
		  send_strides[1]       = 1; send_stride_counts[1] = 2;
		  send_strides[2]       = Z; send_stride_counts[2] = Z;
		  
		  emb_send(1-p,sizeof(int),A,0,3,send_strides, send_stride_counts,0);
		}
	      
	      /* prepare RECEIVES */
	      
	      
	      if (p==RECEIVER)
		{
		  int recv_strides[3],recv_stride_counts[3];
		  
		  recv_strides[0]       = 2; recv_stride_counts[0] = Z/2;
		  recv_strides[1]       = 1; recv_stride_counts[1] = 2;
		  recv_strides[2]       = Z; recv_stride_counts[2] = Z;
		  
		  emb_recv(1-p,sizeof(int),B,0,3,recv_strides, recv_stride_counts,0);
		}
	      
	      /* perform SENDS and RECEIVES */
	      
	      emb_barrier();
	      T1 = timer_value();
	      emb_fence();
	      emb_barrier();
	      T2 = timer_value();
	      
	      if (SHOWFENCE)
		{
		  int N    = 4*Z*Z;
		  double T = timer_double(T2-T1);
		  
		  printf("[%d] --> fence B->B #%06d: %d bytes in %10.5f seconds [%10.3f MB/sec].\n",
			 p,repeat,N,T,(N/1048576.0)/T);
		}
	      
	    }
	} /* end repeat */
	  
      if (p==RECEIVER)
	{
	  int i,j,flag;
	  
	  if (Z<=30) /* print */
	    for(i=0;i<Z;i++)
	      {
		for(j=0;j<Z;j++)
		  if (B[i][j]==0xdeadbeef)
		    printf(" ...");
		  else
		    printf(" %03d",B[i][j]);
		printf("\n");
	      }
	  
	  flag=1;
	  for(i=0;i<Z;i++)
	    {
	      for(j=0;j<Z;j++)
		if (B[i][j]!=(100))
		  flag=0;
	    }
	  printf("OK-FLAG: %d\n",flag);
	  
	}
    }

  emb_done(1);

  return 0;
}
