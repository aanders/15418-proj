#include "queue.cpp"
#include "sortedCollection.h"
#include <pthread.h>

/*
template <class T> bool SortedCollection<T>::comp2(Entry<T> a, Entry<T> b)
{
  return comp(a.val, b.val);
}
*/

template <class T> SortedCollection<T>::SortedCollection(
  bool (*c)(T a, T b))
{
  numUpdates = numTUpdates = numAUpdates = 0;
  comp = c;
  array = Array<T>(comp);
  tree = SimpleTree<T>(comp);
  pthread_create(&handleUpdatesArray, 0);
}

template <class T> void SortedCollection<T>::ins(T t)
{
  numUpdates++;
  Update<T> addition;
  addition.ins = true;
  addition.val = t;
  treeUpdates.ins(addition);
  arrayUpdates.ins(addition);
}


template <class T> void SortedCollection<T>::del(int idx)
{
  numUpdates++;
  Update<T> deletion;
  deletion.ins = false;
  deletion.idx = idx;
  treeUpdates.ins(deletion);
  arrayUpdates.ins(deletion);
}


template <class T> T SortedCollection<T>::lookup(int idx)
{
  while(numAUpdates != numUpdates) {}
  
  return array.lookup(idx);
}

template <class T> void SortedCollection<T>::handleUpdatesArray(void *unused)
{
  Update<T> u;
  while(true)
  {
    while(!arrayUpdates.remove(&u)) {}
    if(!u.ins)
    {
      array.del(u.idx);
    }
    else
    {
      array.ins(u.val);
    }
    numAUpdates++;
  }
}
