#ifndef __sortedCollection_h__
#define __sortedCollection_h__

template <class T> class SortedCollection
{
  public:
  T *bob;
  int (*comp)(T a, T b);
  
  SortedCollection();
  SortedCollection(int (*c)(T a, T b));
};

#endif
