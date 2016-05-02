#include "updates.h"
#include "arrays/array.h"

/* This is version V5 with an update rather than insert buffer
 */

#ifndef __custom_v7_h__
#define __custom_v7_h__

#define V7_EXPAND_CONST 2

//#define V7_DEBUG

#define V7_MAX_UPDATES 500

template <class T> class CustomArrayV7 : public Array<T>
{
  protected:
  int allocated;
  int size;
  T *data;
  T *start;
  
  Updates<T> *updates;
  T *buffer;
  
  int currUpdates;
  int updatesHandled;
  int updatesAcknowledged;
  
  #ifdef V7_DEBUG
  long totalUpdates;
  long timesFlushed[2];
  long maxFlushed;
  #endif
  
  public:
  CustomArrayV7(bool (*c)(T a, T b));
  
  ~CustomArrayV7();
  
  void ins(T a);
  void del(int idx);
  T lookup(int idx);
  void flush(int cause);
  void flush();
  inline bool ready(int numUpdates, int idx);
};

#include "custom_v7.tpp"

#endif
