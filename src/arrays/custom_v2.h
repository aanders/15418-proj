#include "arrays/array.h"

#ifndef __custom_v2_h__
#define __custom_v2_h__

template <class T> class CustomArrayV2 : public Array<T>
{
  protected:
  int allocated;
  int size;
  T *data;
  
  const int INSERT_BUFFER_SIZE = 10;
  T insertions[INSERT_BUFFER_SIZE];
  int numInsertions = 0;
  
  public:
  CustomArray(bool (*c)(T a, T b));
  void ins(T a);
  void del(int idx);
  T lookup(int idx);
  //bool lookupElt(T val);
};

#include "custom_v2.tpp"

#endif
