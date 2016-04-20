#include "queue.h"
#include "arrays/array.h"
#include "trees/simpleTree.h"
#include "trees/rbtree.h"
#include <mutex>
#include <condition_variable>

#ifndef __sortedCollection_h__
#define __sortedCollection_h__

#define TYPE_INSERT 0
#define TYPE_DELETE 1
#define TYPE_STOP 2

template <class T> class Update
{
  public:
  int type;
  T val;
  int idx;
};


template <class T> class SortedCollection
{
  protected:
  bool (*comp)(T a, T b);
  
  Array<T> *array;
  RBTree<T> *tree;
  Queue< Update<T> > treeUpdates;
  Queue< Update<T> > arrayUpdates;
  
  int numUpdates;

  int numAUpdates;
  int numTUpdates;

  std::mutex aUpdatesMutex;
  std::mutex tUpdatesMutex;
  std::mutex atUpdatesMutex;

  std::unique_lock<std::mutex> aUpdatesWait;
  std::unique_lock<std::mutex> tUpdatesWait;
  std::unique_lock<std::mutex> atUpdatesWait;

  std::condition_variable aReady;
  std::condition_variable tReady;
  std::condition_variable atReady;
  
  pthread_t tThread, aThread;
  
  public:
  SortedCollection(bool (*c)(T a, T b));
  ~SortedCollection();
  void ins(T a);
  void del(int idx);
  T lookup(int idx);
  bool lookupElt(T val);
  void remove(T val);
  
  void *handleUpdatesArray();
  void *handleUpdatesTree();
};

#include "sortedCollection.tpp"
#endif
