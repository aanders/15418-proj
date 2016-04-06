#include <set>
#include <vector>
#include "queue.cpp"

#ifndef __sortedCollection_h__
#define __sortedCollection_h__

template <class T> class SortedCollection
{
  private:
  bool (*comp)(T a, T b);
  std::vector<T> vector;
  std::set<T> set;
  
  public:
  SortedCollection(bool (*c)(T a, T b));
  void ins(T a);
  void del(int idx);
  T lookup(int idx);
};

#endif
