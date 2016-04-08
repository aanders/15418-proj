#include "queue.h"
#include "../arrays/vector_v1.h"
#include "../trees/simpleTree.h"

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
  protected:
  bool (*comp)(T a, T b);
  //bool comp2(Entry<T> a, Entry<T> b);
  
  Array<T> array;
  SimpleTree<T> tree;
  Queue< Update<T> > treeUpdates;
  Queue< Update<T> > arrayUpdates;
  
  int numUpdates;
  int numAUpdates;
  int numTUpdates;
  
  void handleUpdatesArray(void *unused);
  
  public:
  SortedCollection(bool (*c)(T a, T b));
  SortedCollection();
  void ins(T a);
  void del(int idx);
  T lookup(int idx);
  bool lookupElt(T val);
};

#endif
