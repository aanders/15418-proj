#include "arrays/array.h"
#include "arrays/custom_v4.h"

/* This is version V4 modified so as to have an insert buffer
 */

#ifndef __custom_v5_h__
#define __custom_v5_h__

#define V5_EXPAND_CONST 2

//#define V5_DEBUG

template <class T> class CustomArrayV5 : public Array<T>
{
  protected:
  int allocated;
  int size;
  T *data;
  T *start;
  
  CustomArrayV4<T> *inserts;
  int numInserts;
  
  #ifdef V5_DEBUG
  long totalInserts;
  long timesFlushed[2];
  long maxFlushed;
  #endif
  
  public:
  CustomArrayV5(bool (*c)(T a, T b));
  
  #ifdef V5_DEBUG
  ~CustomArrayV5();
  #endif
  
  void ins(T a);
  void del(int idx);
  T lookup(int idx);
  void flush(int cause);
  void flush();
  //bool lookupElt(T val);
};

#include "custom_v5.tpp"

#endif
