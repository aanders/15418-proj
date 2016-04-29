#ifndef __updates_h__
#define __updates_h__

#define UPDATE_INSERT 0
#define UPDATE_DELETE 1

/*
template <class T> class Update
{
  public:
  int index;
  int timestamp;
  int type;
  T val;
};
*/

template <class T> class Updates
{
  public:
  T *values;
  int *indices;
  int *types;
  
  int maxSize;
  int size;
  bool (*comp)(T a, T b);
  
  Updates(bool (*c)(T a, T b), int ms);
  void ins(T val, T* array, int asize);
  void del(int idx);
  void empty();
};

#include "updates.tpp"

#endif
