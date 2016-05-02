#include "arrays/array.h"
#include <vector>

#ifndef __vector_v1_h__
#define __vector_v1_h__

template <class T> class VectorArray: public Array<T>
{
  protected:
  std::vector<T> data;
  int updatesHandled;
  
  public:
  VectorArray(bool (*c)(T a, T b));
  void ins(T a);
  void del(int idx);
  T lookup(int idx);
  inline bool ready(int numUpdates, int idx);
  void flush();
};

#include "vector_v1.tpp"

#endif
