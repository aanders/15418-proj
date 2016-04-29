#include "updates.h"
#include "arrays/array.h"

/* This is version V5 with an update rather than insert buffer
 */

#ifndef __custom_v6_h__
#define __custom_v6_h__

#define V6_EXPAND_CONST 2

//#define V6_DEBUG

#define V6_MAX_UPDATES 17

template <class T> class CustomArrayV6 : public Array<T>
{
  protected:
  int allocated;
  int size;
  T *data;
  T *start;
  
  Updates<T> *updates;
  int numUpdates;
  int numInserts;
  
  #ifdef V6_DEBUG
  long totalUpdates;
  long timesFlushed[2];
  long maxFlushed;
  #endif
  
  public:
  CustomArrayV6(bool (*c)(T a, T b));
  
  #ifdef V6_DEBUG
  ~CustomArrayV6();
  #endif
  
  void ins(T a);
  void del(int idx);
  T lookup(int idx);
  void flush(int cause);
  void flush();
  //bool lookupElt(T val);
};

#include "custom_v6.tpp"

#endif
