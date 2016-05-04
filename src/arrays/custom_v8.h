#include "updates.h"
#include "arrays/array.h"

/* This is version is a V4-V6 combo
 */

#ifndef __custom_v8_h__
#define __custom_v8_h__

#define V8_EXPAND_CONST 2

//#define V8_DEBUG

#define V8_MAX_UPDATES 2000
#define V8_SWITCH_POINT 40

#define V8_V4MODE 0
#define V8_V6MODE 1

template <class T> class CustomArrayV8 : public Array<T>
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
  
  #ifdef V8_DEBUG
  long totalUpdates;
  long timesFlushed[2];
  long maxFlushed;
  #endif
  
  public:
  CustomArrayV8(bool (*c)(T a, T b), int *tU);
  
  ~CustomArrayV8();
  
  void ins(T a);
  void del(int idx);
  T lookup(int idx);
  void flush(int cause);
  void flush();
  inline bool ready(int numUpdates, int idx);
};

#include "custom_v8.tpp"

#endif
