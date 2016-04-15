#ifndef __array_h__
#define __array_h__

template <class T> class Array
{
  protected:
  bool (*comp)(T a, T b);
  
  public:
  Array(bool (*c)(T a, T b));
  virtual void ins(T a) =0;
  virtual void del(int idx) =0;
  virtual T lookup(int idx) =0;
  virtual bool lookupElt(T val) =0;
};

#include "array.tpp"

#endif
