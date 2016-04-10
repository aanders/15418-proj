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

template <class T> SortedCollection<T>::SortedCollection(
  bool (*c)(T a, T b))
{
  numUpdates = numTUpdates = numAUpdates = 0;
  comp = c;
  array = new Array<T>(comp);
  tree = new SimpleTree<T>(comp);
  aUpdatesWait = std::unique_lock<std::mutex>(aUpdatesMutex);
  
  pthread_t wat;
  pthread_create(&wat, NULL, arrayThread<T>, this);
}

template <class T> void SortedCollection<T>::ins(T t)
{
  numUpdates++;
  Update<T> addition;
  addition.type = TYPE_INSERT;
  addition.val = t;
  treeUpdates.insert(addition);
  arrayUpdates.insert(addition);
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
    /*
    Update<T> wait;
    wait.type = TYPE_WAIT;
    arrayUpdates.insert(wait);
    */
    aReady.wait(aUpdatesWait);
  }
  return array->lookup(idx);
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
    /*
    else if(u.type == TYPE_WAIT)
    {
      cout<<"Gotta notify."<<endl;
    }
    */
    
    numAUpdates++;
    //cout<<"Job done."<<endl;
  }
  return NULL;
}
