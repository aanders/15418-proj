#ifndef __updates_h__
#define __updates_h__

#define UPDATE_INSERT 0
#define UPDATE_DELETE 1

template <class T> class Updates
{
  public:
  T *values;
  int *indices;
  int *types;
  
  int maxSize;
  int size;
  bool (*comp)(T a, T b);
  
  int firstOverHalf;
  int iuh;
  int last;
  
  Updates(bool (*c)(T a, T b), int ms);
  void ins(T val, T* array, int asize);
  void del(int idx, int asize);
  void empty();
};

#include "updates.tpp"

#endif
