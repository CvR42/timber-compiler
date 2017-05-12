
/*****************/
/** embdmalus.h **/
/*****************/

#ifdef __cplusplus
extern "C" {
#endif

#ifndef EMBDMALUS_H
#define EMBDMALUS_H

#define EMB_FLAG_EXCLUSIVE 1
  
  int emb_init(const int argc, char **argv, const int nProcs, const int useHost);
  void emb_done(const int showSummary);

  void emb_barrier(void);
  int  emb_NP(void);
  int  emb_PID(void);
  
  void emb_send(int dstProc, int elmSize, const void *base, int offset,
		int num_strides, const int *strides, const int *stride_count,
		int flags);
  void emb_recv(int srcProc, int elmSize, const void *base, int offset,
		int num_strides, const int *strides, const int *stride_count,
		int flags);

  void emb_send_block(int dstProc, const void *base, int size, int flags);
  void emb_recv_block(int srcProc, const void *base, int size, int flags);


  void emb_fence(void);

#ifdef EMB_USE_TIMING

#ifdef __TCS__
  typedef unsigned long TimerValue;
#endif
#ifdef __GNUC__
  typedef unsigned long long TimerValue;
#endif

  void timer_init(void);
  void timer_done(void);
  void timer_reset(void);
  double timer_double(TimerValue DT);
  TimerValue timer_value(void);

#endif
#endif  
#ifdef __cplusplus
	   }
#endif
