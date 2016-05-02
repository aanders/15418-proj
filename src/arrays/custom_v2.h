#include "arrays/array.h"

#ifndef __custom_v2_h__
#define __custom_v2_h__

#define INSERT_BUFFER_SIZE 100
//#define DELETE_BUFFER_SIZE 10

template <class T> class CustomArrayV2 : public Array<T>
{
  protected:
  int allocated;
  int size;
  T *data;
  
  T insertions[INSERT_BUFFER_SIZE * 2];
  int numInsertions, updatesHandled;
  
  void sortedChecker(T *d, int s);
  
  void emptyInsertions();
  void printData(T *d, int s);
  
  public:
  CustomArrayV2(bool (*c)(T a, T b));
  void ins(T a);
  void del(int idx);
  T lookup(int idx);
  void flush();
  
  inline bool ready(int numUpdates, int idx);
};

#include "custom_v2.tpp"

#endif
