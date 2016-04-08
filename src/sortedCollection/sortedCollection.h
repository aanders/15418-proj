#include <set>
#include <vector>
#include "queue.cpp"

#ifndef __sortedCollection_h__
#define __sortedCollection_h__

template <class T> class Update
{
  public:
  bool ins;
  T val;
  int idx;
};

template <class T> class Entry
{
  public:
  T val;
  int count;
};

template <class T> class SortedCollection
{
  private:
  bool (*comp)(T a, T b);
  bool comp2(Entry<T> a, Entry<T> b);
  
  std::vector<T> vector;
  std::set< Entry<T> > set;
  Queue< Update<T> > setUpdates;
  Queue< Update<T> > vectorUpdates;
  
  int numUpdates;
  int numVUpdates;
  int numSUpdates;
  
  void handleUpdatesVector();
  
  public:
  SortedCollection(bool (*c)(T a, T b));
  void ins(T a);
  void del(int idx);
  T lookup(int idx);
};

#endif
