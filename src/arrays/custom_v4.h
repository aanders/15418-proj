#include "arrays/array.h"

/* This is version V1 modified so as to have empty space
 * both before and after the data.
 */

#ifndef __custom_v4_h__
#define __custom_v4_h__

#define V4_EXPAND_CONST 2

template <class T> class CustomArrayV4 : public Array<T>
{
  protected:
  int allocated;
  int size;
  T *data;
  T *start;
  
  public:
  CustomArrayV4(bool (*c)(T a, T b));
  void ins(T a);
  void del(int idx);
  T lookup(int idx);
  
  void flush();
  
  //bool lookupElt(T val);
};

#include "custom_v4.tpp"

#endif
