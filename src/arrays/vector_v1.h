#include "arrays/array.h"
#include <vector>

#ifndef __vector_v1_h__
#define __vector_v1_h__

template <class T> class VecArray: public Array<T>
{
  protected:
  //bool (*comp)(T a, T b);
  std::vector<T> data;
  
  public:
  VecArray(bool (*c)(T a, T b));
  void ins(T a);
  void del(int idx);
  T lookup(int idx);
  bool lookupElt(T val);
};

#include "vector_v1.tpp"

#endif
