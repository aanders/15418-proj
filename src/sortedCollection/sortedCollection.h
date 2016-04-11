#include "queue.h"
#include "arrays/vector_v1.h"
#include "trees/simpleTree.h"
#include "trees/rbtree.h"
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
  RBTree<T> *rbtree;
  Queue< Update<T> > treeUpdates;
  Queue< Update<T> > arrayUpdates;
  Queue< Update<T> > rbtreeUpdates;
  
  int numUpdates;

  int numAUpdates;
  int numTUpdates;
  int numRBTUpdates;

  std::mutex aUpdatesMutex;
  std::mutex rbtUpdatesMutex;

  std::unique_lock<std::mutex> aUpdatesWait;
  std::unique_lock<std::mutex> rbtUpdatesWait;

  std::condition_variable aReady;
  std::condition_variable rbtReady;
  
  
  public:
  SortedCollection(bool (*c)(T a, T b));
  SortedCollection();
  void ins(T a);
  void del(int idx);
  T lookup(int idx);
  bool lookupElt(T val);
  void remove(T val);

  void *handleUpdatesArray();
  void *handleUpdatesRBT();
};

#include "sortedCollection.tpp"
#endif
