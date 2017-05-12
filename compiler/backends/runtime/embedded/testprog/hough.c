#include <ensemble.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

#define VERBOSE 2
#define DO_HOUGH 1
#define DO_CHECKSUM 0
#define WRITE_FILES 0
#define REPEAT  1000

#define HPICSIZE 512
#define VPICSIZE 512

#define N_ANGLES 360
#define N_RHO    400
#define MAX_RHO  1000

#define PI (3.14159265358979323846264338327f)

typedef char Picture[VPICSIZE][HPICSIZE];
typedef int  HoughXForm [N_ANGLES][N_RHO];

struct MaxInfo {
  int dummy1[100];
  int value;
  int theta;
  int rho;
  int dummy2[100];
};

float SIN[N_ANGLES];
float COS[N_ANGLES];

int p,np;

static int roundoff(float x)
{
  return (int)(x+0.5f);
}

static int my_rand(void)
{
  static unsigned long SEED=0;

  SEED *= 0x01010101LU;
  SEED += 0x31415927LU;
  return (SEED>>8)&0x7fff;
}

static void picture_clear(Picture P)
{
  memset(P,0,sizeof(Picture));
}


static void picture_plot(Picture P, int x, int y)
{
  if (y>=0 && y<VPICSIZE && x>=0 && x<HPICSIZE)
    P[y][x] = 1;
}

static void picture_draw(Picture P, int x1, int y1, int x2, int y2)
{
    int majordx, minordx;
    int majordy, minordy;
    int numer, denom;
    int steps;
    int swap,dnumer;
    
    /* draw from (x1,y1) to (x2,y2) */
    
    majordx = 1;
    majordy = 0;
    steps   = x2-x1;
    
    if (steps<0)
	{
	    majordx = -majordx;
	    majordy = -majordy;
	    steps   = -steps;
	}
    
    minordx = 0;
    minordy = 1;
    dnumer  = y2-y1;
    
    if (dnumer<0)
	{
	    minordx = -minordx;
	    minordy  = -minordy;
	    dnumer   = -dnumer;
	}
    
    if (dnumer>steps)
	{
	    swap = minordx; minordx = majordx; majordx = swap;
	    swap = minordy; minordy = majordy; majordy = swap;
	    swap = steps  ; steps   = dnumer ; dnumer  = swap;
	}
    
    picture_plot(P,x1,y1);
    
    if (steps>0)
	{
	    numer = steps;
	    
	    if ((majordx+majordy)<0)
		--numer;
	    
	    denom   = steps;
	    denom  += denom;
	    dnumer += dnumer;
	    
	    do
		{
		    
		    x1+=majordx;
		    y1+=majordy;
		    numer-=dnumer;
		    
		    if (numer<0)
			{
			    numer+=denom;
			    x1+=minordx;
			    y1+=minordy;
			}
		    
		    picture_plot(P,x1,y1);
		    
		    --steps;
		}
	    while (steps>0);
	}
}   

static void picture_checksum(Picture P)
{
  int x,y,s;

  s=0;
  for(y=0;y<VPICSIZE;y++)
    for(x=0;x<HPICSIZE;x++)
      if (P[y][x])
	++s;
      
  printf("[%d] picture checksum: %d\n",p,s);
}

static void houghxform_init(void)

{
  int theta;

  for(theta=0;theta<N_ANGLES;theta++)
    {
      SIN[theta] = (float)sin(theta*PI/N_ANGLES);
      COS[theta] = (float)cos(theta*PI/N_ANGLES);
    }
}

static void houghxform_clear(HoughXForm H)
{
  int theta,rho;
  for(theta=p;theta<N_ANGLES;theta+=np)
    for(rho=0;rho<N_RHO;rho++)
      {
	H[theta][rho]=0;
      }
}

static void houghxform_write(HoughXForm H)
{
  char fname[100];
  int theta,rho,maxval;
  FILE *f;

  maxval=-1;

  for(theta=0;theta<N_ANGLES;theta++)
    {
      for(rho=0;rho<N_RHO;rho++)
	{
	  if (H[theta][rho]>maxval)
	    maxval = H[theta][rho];
	}
    }

  if (maxval<1)
    maxval=1;

  sprintf(fname,"hough%d.pgm",p);
  f = fopen(fname,"w");
  assert(f);

  fprintf(f,"P2\n");
  fprintf(f,"%d %d\n",N_RHO,N_ANGLES);
  fprintf(f,"%d\n",maxval);

  for(theta=0;theta<N_ANGLES;theta++)
    {
      if (theta%10==0) printf("[%d] theta: %d\n",p,theta);
      for(rho=0;rho<N_RHO;rho++)
	{
	  fprintf(f,"%d\n",maxval-H[theta][rho]);
	}
    }

  fclose(f);
}

static void picture_add_noise(Picture P, int points)
{
  int x,y;

  while(points--)
    {
      x = (my_rand()^my_rand()^my_rand()) % HPICSIZE;
      y = (my_rand()^my_rand()^my_rand()) % VPICSIZE;

      P[y][x] ^= 1;
    }
}

static void picture_write(Picture P)
{
  char fname[100];
  int x,y,maxval;
  FILE *f;

  maxval=-1;

  for(y=0;y<VPICSIZE;y++)
    for(x=0;x<HPICSIZE;x++)
      {
	if (P[y][x]>maxval)
	  maxval = P[y][x];
      }

  if (maxval<1)
    maxval=1;

  sprintf(fname,"picture%d.pgm",p);
  f = fopen(fname,"w");
  assert(f);

  fprintf(f,"P2\n");
  fprintf(f,"%d %d\n",HPICSIZE,VPICSIZE);
  fprintf(f,"%d\n",maxval);

  for(y=0;y<VPICSIZE;y++)
    for(x=0;x<HPICSIZE;x++)
      {
	fprintf(f,"%d\n",maxval-P[y][x]);
      }

  fclose(f);
}

static void picture_add_line(Picture P)
{
  int x1,y1,x2,y2;

  float theta = 1.5;
  float rho   = 42.0;
  float k     = 400.0;

  x1 = roundoff(cos(theta)*rho-sin(theta)*k+0.5f*(HPICSIZE-1));
  y1 = roundoff(sin(theta)*rho+cos(theta)*k+0.5f*(VPICSIZE-1));

  x2 = roundoff(cos(theta)*rho+sin(theta)*k+0.5f*(HPICSIZE-1));
  y2 = roundoff(sin(theta)*rho-cos(theta)*k+0.5f*(VPICSIZE-1));

  if (VERBOSE>=1)
    printf("adding line (%d,%d)--(%d,%d) theta=%10.3f rho=%10.4f\n",
	   x1,y1,x2,y2,theta,rho);

  picture_draw(P,x1,y1,x2,y2);

}

static void picture_to_hough(Picture P, HoughXForm H)
{
  int x,y;
  int theta;

  for(y=0;y<VPICSIZE;y++)
    for(x=0;x<HPICSIZE;x++)
      if (P[y][x])
	{
	  for(theta=p;theta<N_ANGLES;theta+=np)
	    {
	      float px,py;
	      float rho;
	      int int_rho;
	      
	      /*
	       * map topleft-based coordinates --> center-based coordinates
	       * x = 0..(HPICSIZE-1)   -->  px =    -(HPICSIZE/2) .. (HPICSIZE/2)
	       * y = 0..(VPICSIZE-1)   -->  py =    -(VPICSIZE/2) .. (VPICSIZE/2)
	       *
	       * inverse mapping is given by
	       *
	       *
	       * x = px + 0.5f*(HPIC_SIZE-1);
	       * y = py + 0.5f*(HPIC_SIZE-1);
	       *
	       */
	      
	      px = x-0.5f*(HPICSIZE-1);
	      py = y-0.5f*(VPICSIZE-1);
	      
	      rho = px*COS[theta] + py*SIN[theta];
	      
	      /*
	       * map rho --> 0..MAX_RHO-1
	       *
	       * rho = -MAX_RHO --> int_rho =    0
	       * rho =  0       --> int_rho =    (N_RHO-1)/2
	       * rho =  MAX_RHO --> int_rho =    (N_RHO-1)
	       *
	       * inverse mapping is given by
	       *
	       * rho = (2.0*int_rho/(N_RHO-1)-1.0)*MAX_RHO;
	       *
	       */
	      
	      int_rho = roundoff(0.5f*(rho/MAX_RHO+1.0f)*(N_RHO-1));
	      
	      ++H[theta][int_rho];
	      
	    }
	}
}

static void hough_detect(HoughXForm H, struct MaxInfo *maxinfo)
{
  int theta,rho;

  maxinfo->value = -1;

  for(theta=p;theta<N_ANGLES;theta+=np)
    for(rho=0;rho<N_RHO;rho++)
      if (H[theta][rho]>maxinfo->value)
	{
	  maxinfo->value = H[theta][rho];
	  maxinfo->theta = theta;
	  maxinfo->rho   = rho;
	}

  if (VERBOSE>=2)
    printf("[%d] LOCAL--MAX line detected: theta = %10.3f rho=%10.3f value=%d\n",p,
	   (maxinfo->theta*PI/N_ANGLES),
	   (2.0f*maxinfo->rho/(N_RHO-1)-1.0f)*MAX_RHO,
	   maxinfo->value
	   );
}

Picture      pic;
HoughXForm xform;

int main(int argc, char ** argv)

{
  int R;
  clock_t T1,T2;
  float T;
  struct MaxInfo MAX[10];

  emb_init(argc, argv, 5,1);

  p = emb_PID();
  np = emb_NP();

  /* init SINE tables */

  houghxform_init();

  /* The source picture is LOCAL-0 */

  if (p==0)
    {
      picture_clear(pic);
      picture_add_line(pic);
      picture_add_noise(pic,HPICSIZE*VPICSIZE/20);
    }

  emb_barrier();

  T1 = clock();

  for(R=1;R<=REPEAT;R++)
    {

      /* broadcast the picture from processor 0 to others. */

      if (p!=0)
	{
	  const int strides[2]={1,HPICSIZE};
	  const int counts [2]={HPICSIZE,VPICSIZE};
	  emb_recv(0,sizeof(char),pic,0,2,strides,counts,1);
	}
      else
	{
	  const int strides[2]={1,HPICSIZE};
	  const int counts [2]={HPICSIZE,VPICSIZE};

	  int peer;

	  for(peer=1;peer<np;peer++)
	    emb_send(peer,sizeof(char),pic,0,2,strides,counts,1);
	}

      emb_fence();

      if (DO_CHECKSUM)
	picture_checksum(pic);

      if (DO_HOUGH)
	{

	  /* all processors need to CLEAR; this array is replicated */
	  /* this is parallellized in THETA */
	  houghxform_clear(xform);

	  /* this is parallellized in THETA */
	  picture_to_hough(pic,xform);

	  if (WRITE_FILES)
	    {
	      picture_write(pic);
	      houghxform_write(xform);
	    }
	  
	  /* this is parallellized in THETA */
	  hough_detect(xform,&MAX[p]);

	  if (p!=0)
	    {
	      emb_send_block(0,&MAX[p],sizeof(struct MaxInfo),1);
	    }
	  else
	    {
	      int peer;
	      for(peer=1;peer<np;peer++)
		emb_recv_block(peer,&MAX[peer],sizeof(struct MaxInfo),1);
	    }


	  emb_fence();
	  
	  if (p==0)
	    {
	      int peer,max_value=-1,max_peer=-1;
	      for(peer=0;peer<np;peer++)
		{
		  if (MAX[peer].value > max_value)
		    {
		      max_value = MAX[peer].value;
		      max_peer  = peer;
		    }
		}
	      if (VERBOSE>0)
		printf("[%d] GLOBAL-MAX line detected: theta = %10.3f rho=%10.3f value=%d (RUN #%d)\n",p,
		       (MAX[max_peer].theta*PI/N_ANGLES),
		       (2.0f*MAX[max_peer].rho/(N_RHO-1)-1.0f)*MAX_RHO,
		       MAX[max_peer].value,R
		       );
	    }
	}
    }
  
  T2 = clock();

  T = (float)(T2-T1)/CLOCKS_PER_SEC;

  emb_barrier();

  printf("[%d] time: %.3f seconds.\n",p,T);

  emb_done(1);

  return 0;
}
