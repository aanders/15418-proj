#include "simpleTree.h"
#include<iostream>

using namespace std;

template <class T> SimpleTree<T>::SimpleTree(bool (*c)(T a, T b))
{
  l = 0;
  r = 0;
  size = 0;
  comp = c;
}

template <class T> SimpleTree<T>::~SimpleTree()
{
  if(l != 0)
    delete l;
  if(r != 0)
    delete r;
}

template <class T> void SimpleTree<T>::ins(T t)
{
  if(size == 0)
  {
    val = t;
  }
  else
  {
    if(comp(val, t))
    {
      if(l == 0)
        l = new SimpleTree(comp);
      l->ins(val);
    }
    else
    {
      if(r == 0)
        r = new SimpleTree(comp);
      r->ins(val);
    }
  }
  size++;
}

template <class T> T SimpleTree<T>::lookup(int idx)
{
  if(idx < 0 || idx > (size - 1))
  {
    cout<<"Error: invalid index"<<endl;
    return *(new T);
  }
  
  if(l == 0)
  {
    if(idx == 0)
      return val;
    return r->lookup(idx - 1);
  }
  if(idx < l->size)
    return l->lookup(idx);
  return r->lookup(idx - (1 + l->size));
}

template <class T> void SimpleTree<T>::del(int idx)
{
  if(idx < 0 || idx > (size - 1))
  {
    cout<<"Error: invalid index"<<endl;
    return;
  }
  
  if(l == 0 && idx == 0)
  {
    if(r == 0)
      size = 0;
    else
    {
      size = r->size;
      l = r->l;
      val = r->val;
      SimpleTree *newR = r->r;
      r->l = 0;
      r->r = 0;
      delete r;
      r = newR;
    }
  }
  
  SimpleTree *eligible = this;
  while(!(eligible->l == 0 && idx == 0))
  {
    
  }
}
