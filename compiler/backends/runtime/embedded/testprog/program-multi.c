
#define EMB_USE_TIMING
#include <ensemble.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#define Z 200
#define SHOWFENCE 1
#define USE_BARRIER 0
#define REPEAT 10000

#ifdef __TCS__
#include <tmlib/tmlibc.h>
#  define cache_malloc(length) _cache_malloc(length,-1)
#  define cache_free(ptr)      _cache_free(ptr)
#endif
#ifdef __GNUC__
#  define cache_malloc(length) malloc(length)
#  define cache_free(ptr)      free(ptr)
#endif

typedef int Array[Z][Z];

/* this program does an ALL-GATHER of a CYCLIC-DISTRIBUTED array to all processors */

/*   / 0 0 0 0 0 0 0 0 0 0 \    / 0 0 0 0 0 0 0 0 0 0 \
 *   | 1 1 1 1 1 1 1 1 1 1 |    | 0 0 0 0 0 0 0 0 0 0 |
 *   | 2 2 2 2 2 2 2 2 2 2 |    | 0 0 0 0 0 0 0 0 0 0 |
 *   | 3 3 3 3 3 3 3 3 3 3 |    | 1 1 1 1 1 1 1 1 1 1 |
 *   | 0 0 0 0 0 0 0 0 0 0 | -> | 1 1 1 1 1 1 1 1 1 1 |
 *   | 1 1 1 1 1 1 1 1 1 1 |    | 1 1 1 1 1 1 1 1 1 1 |
 *   | 2 2 2 2 2 2 2 2 2 2 |    | 2 2 2 2 2 2 2 2 2 2 |
 *   | 3 3 3 3 3 3 3 3 3 3 |    | 2 2 2 2 2 2 2 2 2 2 |
 *   | 0 0 0 0 0 0 0 0 0 0 |    | 3 3 3 3 3 3 3 3 3 3 |
 *   \ 1 1 1 1 1 1 1 1 1 1 /    \ 3 3 3 3 3 3 3 3 3 3 /
 */

int howmany(int A, int B, int OFFSET, int PERIOD)
{
  A = A-OFFSET-1;
  B = B-OFFSET;

  if (A<0) A=(A-PERIOD+1); /* account for weird C semantics */
  if (B<0) B=(B-PERIOD+1);

  return B/PERIOD - A/PERIOD;
}

int p,np;

void all_gather(void)
{
  int pr,repeat,i,j,nr_rows[10],first_row[10];

  Array A,B;
  
  for(i=0;i<Z;i++)
    for(j=0;j<Z;j++)
      A[i][j]=100+p;
  
  for(i=0;i<Z;i++)
    for(j=0;j<Z;j++)
      B[i][j]=999;
  
  for(i=0;i<np;i++)
    nr_rows[i] = howmany(0,Z-1,i,np);
  first_row[0] = 0;
  for(i=1;i<np;i++)
    first_row[i] = first_row[i-1]+nr_rows[i-1];
  
  for(repeat=1;repeat<=10;repeat++)
    {
      
      /* prepare SENDS */
      
      if (1)
	{
	  /* how many do I own? */
	  
	  if (nr_rows[p]>0) /* at least one -- I need to send! */
	    {
	      int send_strides[2],send_stride_counts[2];
	      
	      send_strides[0] =    1; send_stride_counts[0] =  Z ;
	      send_strides[1] = np*Z; send_stride_counts[1] = nr_rows[p];
	      
	      for (pr=0;pr<np;pr++)
		{
		    emb_send(pr,sizeof(int),A,p*Z,2,send_strides, send_stride_counts,0);
		  if (0)
		    printf("[%d] send %d to %d\n",p,Z*nr_rows[p]*4,pr);
		}
	    }
	}
      
      /* prepare RECEIVES */

      for(pr=0;pr<np;pr++)
	{
	  if (nr_rows[pr]>0)
	    {
	      int recv_strides[2],recv_stride_counts[2];
	      
	      recv_strides[0] = 1; recv_stride_counts[0]=Z;
	      recv_strides[1] = Z; recv_stride_counts[1]= nr_rows[pr];
	      
	      emb_recv(pr,sizeof(int),B,first_row[pr]*Z,2,recv_strides, recv_stride_counts,0);
	      if (0)
		printf("[%d] receive %d from %d\n",p,Z*nr_rows[pr]*4,pr);
	    }
	}
      
      /* perform SENDS and RECEIVES */
      
      emb_barrier();
      emb_fence();
	  
    } /* end repeat */

  if (0)
    {
      int i,j,flag;
      
      if (1) /* print */
	for(i=0;i<Z;i++)
	  {
	    for(j=0;j<Z;j++)
	      printf("%6d",B[i][j]);
	    printf("\n");
	  }
      
      flag=1;
      for(i=0;i<Z;i++)
	    {
	      for(j=0;j<Z;j++)
		if(j%np!=0)
		  if (B[i][j]!=(100+j%np))
		    flag=0;
	    }
      printf("OK-FLAG: %d\n",flag);
    }
}

void all_to_all(void)
{
  int r;
  Array *A,*B[10];

  int B_strides[2],B_stride_counts[2];
  int U_strides[2],U_stride_counts[2];
  
  B_strides[0] = Z; B_stride_counts[0] = Z;
  B_strides[1] = 1; B_stride_counts[1] = Z;

  U_strides[0] = 1; U_stride_counts[0] = Z;
  U_strides[1] = Z; U_stride_counts[1] = Z;

  int i,j,repeat,pr;
  TimerValue T1,T2;
  
  int B_strides[3],B_stride_counts[3];
  int U_strides[3],U_stride_counts[3];
  
  B_strides[0] = 2; B_stride_counts[0] = Z/2;
  B_strides[1] = 1; B_stride_counts[1] = 2;
  B_strides[2] = Z; B_stride_counts[2] = Z;
  
  U_strides[0] = 1; U_stride_counts[0] = Z;
  U_strides[1] = Z; U_stride_counts[1] = Z;
  
  A = cache_malloc(sizeof(Array));
  assert(A);
  for(i=0;i<np;i++)
    {
      B[i] = cache_malloc(sizeof(Array));
      assert(B[i]);
    }
  
  for(i=0;i<Z;i++)
    for(j=0;j<Z;j++)
      A[0][i][j]=100*p+10*i+j;
  
  for(pr=0;pr<np;pr++)
    for(i=0;i<Z;i++)
      for(j=0;j<Z;j++)
	B[pr][0][i][j]=999;
  
  for(repeat=1;repeat<=REPEAT;repeat++)
    {
      
      for (pr=0;pr<np;pr++)
	  {
	    for(r=1;r<=10;r++)
	      {
		int smode,rmode;
		
		smode = 0;
		rmode = 1;
		
		if (smode)
		  emb_send(pr,sizeof(int),A[0] ,0,3,
			   B_strides, B_stride_counts,0);
		else
		  emb_send(pr,sizeof(int),A[0] ,0,2,
			   U_strides, U_stride_counts,0);
		
		if (rmode)
		  emb_recv(pr,sizeof(int),B[pr] ,0,3,
			   B_strides, B_stride_counts,0);
		else
		  emb_recv(pr,sizeof(int),B[pr] ,0,2,
			   U_strides, U_stride_counts,0);
	      }
	  }

      if (USE_BARRIER)
	emb_barrier();

      T1 = timer_value();

      emb_fence();

      if (USE_BARRIER)
	emb_barrier();

      T2 = timer_value();

      /* this was an all-to-all broadcast:
       * - each processors sends (np-1) matrices.
       * total bytes: (4*Z*Z)*np*(np-1).
       */

      if (SHOWFENCE && repeat%10 ==0 && (repeat/10)%np==p)
      {
	int N    = 10*4*Z*Z*np*(np-1);
	double T = timer_double(T2-T1);
	double B = (N/1048576.0)/T;

	printf("[%d] --> fence #%06d: %d bytes, %10.3f sec, %10.5f MB/sec\n",
	       p,repeat,N,T,B);
	fflush(stdout);
      }

    } /* end repeat */

  if (Z<=100)
    for(pr=0;pr<np;pr++)
      {
	for(i=0;i<Z;i++)
	  {
	    printf("[%d] (from %2d, row #%2d)",p,pr,i);
	    for(j=0;j<Z;j++)
	      printf(" %03d",B[pr][0][i][j]);
	    printf("\n");
	  }
      }

  cache_free(A);
  for(i=0;i<np;i++)
    {
      cache_free(B[i]);
    }
}

int main(int argc, char **argv)

{
  emb_init(argc, argv, 4, 0);

  p = emb_PID();
  np = emb_NP();

  all_to_all();

  emb_done(1);

  return 0;
}
