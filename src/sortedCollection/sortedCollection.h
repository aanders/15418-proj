#include "queue.h"
#include "arrays/vector_v1.h"
#include "trees/simpleTree.h"
#include <mutex>
#include <condition_variable>

#ifndef __sortedCollection_h__
#define __sortedCollection_h__

#define TYPE_INSERT 0
#define TYPE_DELETE 1

template <class T> class Update
{
  public:
  int type;
  T val;
  int idx;
};

/*
template <class T> class Entry
{
  public:
  T val;
  int count;
};
*/

template <class T> class SortedCollection
{
  protected:
  bool (*comp)(T a, T b);
  //bool comp2(Entry<T> a, Entry<T> b);
  
  Array<T> *array;
  SimpleTree<T> *tree;
  Queue< Update<T> > treeUpdates;
  Queue< Update<T> > arrayUpdates;
  
  int numUpdates;
  int numAUpdates;
  std::mutex aUpdatesMutex;
  std::unique_lock<std::mutex> aUpdatesWait;
  std::condition_variable aReady;
  
  int numTUpdates;
  
  public:
  void *handleUpdatesArray();
  SortedCollection(bool (*c)(T a, T b));
  SortedCollection();
  void ins(T a);
  void del(int idx);
  T lookup(int idx);
  bool lookupElt(T val);
};

#include "sortedCollection.tpp"
#endif
