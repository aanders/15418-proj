#ifndef __simpleTree_h__
#define __simpleTree_h__

template <class T> class SimpleTree
{
  protected:
  SimpleTree<T> *l;
  SimpleTree<T> *r;
  int size;
  T val;
  
  bool (*comp)(T a, T b);
   
  public:
  SimpleTree(bool (*c)(T a, T b));
  ~SimpleTree();
  
  void ins(T a);
  void del(int idx);
  T lookup(int idx);
};

#include "simpleTree.tpp"
#endif
