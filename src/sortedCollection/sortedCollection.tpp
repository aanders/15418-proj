#include "queue.h"
#include "sortedCollection.h"
#include "arrays/vector_v1.h"
#include "trees/simpleTree.h"
#include <pthread.h>

/*
template <class T> bool SortedCollection<T>::comp2(Entry<T> a, Entry<T> b)
{
  return comp(a.val, b.val);
}
*/

template <class T> void *arrayThread(void *sc)
{
  return ((SortedCollection<T>*) sc)->handleUpdatesArray(NULL);
}

template <class T> SortedCollection<T>::SortedCollection(
  bool (*c)(T a, T b))
{
  numUpdates = numTUpdates = numAUpdates = 0;
  comp = c;
  array = new Array<T>(comp);
  tree = new SimpleTree<T>(comp);
  // TODO: FIXME: this line causes major problems
  pthread_t wat;
  pthread_create(&wat, NULL, arrayThread<T>, this);
}

template <class T> void SortedCollection<T>::ins(T t)
{
  numUpdates++;
  Update<T> addition;
  addition.ins = true;
  addition.val = t;
  treeUpdates.insert(addition);
  arrayUpdates.insert(addition);
}


template <class T> void SortedCollection<T>::del(int idx)
{
  numUpdates++;
  Update<T> deletion;
  deletion.ins = false;
  deletion.idx = idx;
  treeUpdates.insert(deletion);
  arrayUpdates.insert(deletion);
}


template <class T> T SortedCollection<T>::lookup(int idx)
{
  while(numAUpdates != numUpdates) {}
  
  return array->lookup(idx);
}

template <class T> void *SortedCollection<T>::handleUpdatesArray(void *arg)
{
  arg = 2 + (int*) arg;
  
  Update<T> u;
  while(true)
  {
    
    while(!arrayUpdates.remove(&u)) {}
    
    if(!u.ins)
    {
      array->del(u.idx);
    }
    else
    {
      array->ins(u.val);
    }
    numAUpdates++;
    
  }
  return NULL;
}
