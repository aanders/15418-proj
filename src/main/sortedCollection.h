#include <set>
#include <vector>

#ifndef __sortedCollection_h__
#define __sortedCollection_h__

template <class T> class SortedCollection
{
  private:
  int (*comp)(T a, T b);
  std::vector<T> vector;
  std::set<T, comp> set;
  
  public:
  SortedCollection(int (*c)(T a, T b));
  void ins(T a);
  void del(int idx);
  T lookup(int idx);
};

#endif
