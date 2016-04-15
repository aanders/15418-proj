#include "arrays/array.h"

#ifndef __custom_v1_h__
#define __custom_v1_h__

template <class T> class CustomArray : public Array<T>
{
  protected:
  int allocated;
  int size;
  T *data;
  
  public:
  CustomArray(bool (*c)(T a, T b));
  void ins(T a);
  void del(int idx);
  T lookup(int idx);
  //bool lookupElt(T val);
};

#include "custom_v1.tpp"

#endif
