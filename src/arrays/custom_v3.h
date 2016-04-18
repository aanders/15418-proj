#include "arrays/array.h"

#ifndef __custom_v3_h__
#define __custom_v3_h__

#define AV3_UPDATE_BUFFER_SIZE 20
#define AV3_INS 0
#define AV3_DEL 1

template <class T> class UpdateNode
{
  public:
  int type;
  int idx;
  UpdateNode *next;
  T val;
};

template <class T> class CustomArrayV3 : public Array<T>
{
  protected:
  int allocated;
  int size;
  T *data;
  
  UpdateNode<T> *notUpdates;
  UpdateNode<T> *updates;
  
  void executeUpdates();
  void printData(T *d, int s);
  
  public:
  CustomArrayV3(bool (*c)(T a, T b));
  void ins(T a);
  void del(int idx);
  T lookup(int idx);
  //bool lookupElt(T val);
};

#include "custom_v3.tpp"

#endif
