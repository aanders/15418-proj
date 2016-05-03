#include "queue.h"
#include "sortedCollection.h"
#include "arrays/array.h"
#include "arrays/vector_v1.h"
#include "arrays/custom_v1.h"
#include "arrays/custom_v2.h"
#include "arrays/custom_v3.h"
#include "arrays/custom_v4.h"
#include "arrays/custom_v5.h"
#include "arrays/custom_v6.h"
#include "arrays/custom_v7.h"
#include "trees/simpleTree.h"
#include <pthread.h>
#include <mutex>

template <class T> void *arrayThread(void *sc)
{
  return ((SortedCollection<T>*) sc)->handleUpdatesArray();
}

template <class T> void *treeThread(void *sc)
{
  return ((SortedCollection<T>*) sc)->handleUpdatesTree();
}

template <class T> SortedCollection<T>::SortedCollection(
  bool (*c)(T a, T b))
{
  comp = c;
  numUpdates = numTUpdates = numAUpdates = 0;
  servicedFromTree = servicedFromArray = 0;
  numTimesWaitedOnLookup = 0;
   
  array = new CustomArrayV7<T>(comp);
  tree = new RBTree<T>(comp);
  aUpdatesWait = std::unique_lock<std::mutex>(aUpdatesMutex);
  tUpdatesWait = std::unique_lock<std::mutex>(tUpdatesMutex);
  atUpdatesWait = std::unique_lock<std::mutex>(atUpdatesMutex);
  
  pthread_create(&aThread, NULL, arrayThread<T>, this);
  pthread_create(&tThread, NULL, treeThread<T>, this);
}

template <class T> SortedCollection<T>::~SortedCollection()
{
  Update<T> stop;
  stop.type = TYPE_STOP;
  treeUpdates.insert(stop);
  arrayUpdates.insert(stop);
  
  cout<<"Serviced from tree: \t"<<servicedFromTree<<endl;
  cout<<"Serviced from array: \t"<<servicedFromArray<<endl;
  cout<<"Times it wasn't ready: \t"<<numTimesWaitedOnLookup<<endl;
  
  
  if(pthread_join(aThread, NULL) != 0)
    cout<<"Join failed."<<endl;
  if(pthread_join(tThread, NULL) != 0)
    cout<<"Join failed."<<endl;
  #ifdef V6_DEBUF
  delete (CustomArrayV6<T>*) array;
  #endif
  delete tree;
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
  bool ready = true;
  while(numTUpdates != numUpdates && !array->ready(numUpdates, idx)) 
  {
    if(ready)
    {
      ready = false;
      numTimesWaitedOnLookup++;
    }
    atReady.wait(atUpdatesWait);
  }
  
  if(array->ready(numUpdates, idx))
  {
    servicedFromArray++;
    return array->lookup(idx);
  }
  servicedFromTree++;
  return tree->lookupByIdx(idx)->val;
}

template <class T> bool SortedCollection<T>::lookupElt(T val)
{
  while(numTUpdates != numUpdates)
  {
    tReady.wait(tUpdatesWait);
  }
  return (tree->lookup(val) != nullptr);
}

template <class T> void *SortedCollection<T>::handleUpdatesArray()
{
  //int numHandled = 0;
  Update<T> u;
  while(true)
  {
    while(!arrayUpdates.remove(&u)) 
    {
      array->flush();
      //numAUpdates += numHandled;
      //numHandled = 0;
      atReady.notify_all();
      aReady.notify_all();
    }
    if(u.type == TYPE_DELETE)
    {
      array->del(u.idx);
    }
    else if(u.type == TYPE_INSERT)
    {
      array->ins(u.val);
    }
    else if(u.type == TYPE_STOP)
    {
      return NULL;
    }
    
    //numHandled++;
  }
  return NULL;
}

template <class T> void *SortedCollection<T>::handleUpdatesTree()
{
  Update<T> u;
  
  while(true)
  {
    while(!treeUpdates.remove(&u))
    {
      atReady.notify_all();
      tReady.notify_all();
    }
    
    if(u.type == TYPE_DELETE)
    {
      tree->removeByIdx(u.idx);
    }
    else if(u.type == TYPE_INSERT)
    {
      tree->insert(u.val);
    }
    else if(u.type == TYPE_STOP)
    {
      return NULL;
    }

    numTUpdates++;
  }
  return NULL;
}
