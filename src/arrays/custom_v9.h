#include "updates.h"
#include "arrays/array.h"

/* This is version is a V4-V7 combo
 */

#ifndef __custom_v9_h__
#define __custom_v9_h__

#define V9_EXPAND_CONST 2

//#define V9_DEBUG

#define V9_MAX_UPDATES 2000
#define V9_SWITCH_POINT 40

#define V9_V4MODE 0
#define V9_V7MODE 1

template <class T> class CustomArrayV9 : public Array<T>
{
  protected:
  int allocated;
  int size;
  T *data;
  T *start;
  
  int mode;
  int *targetUpdates;
  
  Updates<T> *updates;
  T *buffer;
  
  int currUpdates;
  int updatesHandled;
  
  int updatesAcknowledged;
  int iuh;
  //volatile int64_t iuhAndStart;
  mutex handyLock;
  bool mainHasLock;
  int w0, w1, w2, w3;
  //^^^ params identifying the "window" of available lookups
  //0 <---> w1e, w2i <---> w3e
  
  #ifdef V9_DEBUG
  long totalUpdates;
  long timesFlushed[2];
  long maxFlushed;
  #endif
  
  public:
  CustomArrayV9(bool (*c)(T a, T b), int *tU);
  
  ~CustomArrayV9();
  
  void ins(T a);
  void del(int idx);
  T lookup(int idx);
  void flush(int cause);
  void flush();
  inline bool ready(int numUpdates, int idx);
};

#include "custom_v9.tpp"

#endif
