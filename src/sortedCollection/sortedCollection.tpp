#include "queue.h"
#include "sortedCollection.h"
#include "arrays/vector_v1.h"
#include "trees/simpleTree.h"
#include <pthread.h>
#include <mutex>

/*
template <class T> bool SortedCollection<T>::comp2(Entry<T> a, Entry<T> b)
{
  return comp(a.val, b.val);
}
*/

template <class T> void *arrayThread(void *sc)
{
  return ((SortedCollection<T>*) sc)->handleUpdatesArray();
}

template <class T> void *rbtThread(void *sc)
{
  return ((SortedCollection<T>*) sc)->handleUpdatesRBT();
}

template <class T> SortedCollection<T>::SortedCollection(
  bool (*c)(T a, T b))
{
  numUpdates = numTUpdates = numAUpdates = 0;
  comp = c;
  array = new Array<T>(comp);
  tree = new SimpleTree<T>(comp);
  rbtree = new RBTree<T>(comp);
  aUpdatesWait = std::unique_lock<std::mutex>(aUpdatesMutex);
  rbtUpdatesWait = std::unique_lock<std::mutex>(rbtUpdatesMutex);
  
  pthread_t wat, morewat;
  pthread_create(&wat, NULL, arrayThread<T>, this);
  pthread_create(&morewat, NULL, rbtThread<T>, this);
}

template <class T> void SortedCollection<T>::ins(T t)
{
  numUpdates++;
  Update<T> addition;
  addition.type = TYPE_INSERT;
  addition.val = t;
  treeUpdates.insert(addition);
  arrayUpdates.insert(addition);
  rbtreeUpdates.insert(addition);
}


template <class T> void SortedCollection<T>::del(int idx)
{
  numUpdates++;
  Update<T> deletion;
  deletion.type = TYPE_DELETE;
  deletion.idx = idx;
  treeUpdates.insert(deletion);
  arrayUpdates.insert(deletion);
}


template <class T> T SortedCollection<T>::lookup(int idx)
{
  while(numAUpdates != numUpdates) 
  {
    aReady.wait(aUpdatesWait);
  }
  return array->lookup(idx);
}

template <class T> bool SortedCollection<T>::lookupElt(T val)
{
  while(numRBTUpdates != numUpdates)
  {
    rbtReady.wait(rbtUpdatesWait);
  }
  return (rbtree->lookup(val) != nullptr);
}

template <class T> void *SortedCollection<T>::handleUpdatesArray()
{
  Update<T> u;
  while(true)
  {
    while(!arrayUpdates.remove(&u)) 
    {
      aReady.notify_all();
    }
    //cout<<"Got job."<<endl;
    if(u.type == TYPE_DELETE)
    {
      //cout<<"It's a deletion."<<endl;
      array->del(u.idx);
    }
    else if(u.type == TYPE_INSERT)
    {
      //cout<<"It's an insertion."<<endl;
      array->ins(u.val);
    }
    
    numAUpdates++;
    //cout<<"Job done."<<endl;
  }
  return NULL;
}

template <class T> void *SortedCollection<T>::handleUpdatesRBT()
{
  Update<T> u;
  while(true)
  {
    while(!rbtreeUpdates.remove(&u))
    {
      rbtReady.notify_all();
    }
    if(u.type == TYPE_DELETE)
    {
      cerr<<"WARNING: rbtree does not support lookup by index"<<endl;
      //rbtree->remove(u.val);
    }
    else if(u.type == TYPE_INSERT)
    {
      rbtree->insert(u.val);
    }

    numRBTUpdates++;
  }
  return NULL;
}
