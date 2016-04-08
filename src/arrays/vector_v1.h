#include<vector>

#ifndef __vector_v1_h__
#define __vector_v1_h__

template <class T> class Array
{
  protected:
  std::vector<T> data;
  bool (*comp)(T a, T b);
  
  public:
  Array(bool (*c)(T a, T b));
  void ins(T a);
  void del(int idx);
  T lookup(int idx);
  bool lookupElt(T val);
};

#endif
