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
#include "arrays/custom_v8.h"
#include "arrays/custom_v9.h"
#include "trees/simpleTree.h"
#include <pthread.h>
#include <mutex>


/*
 * CHANGE THIS DEFINITION TO ALTER THE INTERNAL ARRAY VERSION
 */
#define ARRAY_VERSION 9
//#define TREE_ONLY

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
   
#if ARRAY_VERSION == 9
  array = new CustomArrayV9<T>(comp, &numUpdates);
#elif ARRAY_VERSION == 8
  array = new CustomArrayV8<T>(comp, &numUpdates);
#elif ARRAY_VERSION == 7
  array = new CustomArrayV7<T>(comp);
#elif ARRAY_VERSION == 6
  array = new CustomArrayV6<T>(comp);
#elif ARRAY_VERSION == 5
  array = new CustomArrayV5<T>(comp);
#elif ARRAY_VERSION == 4
  array = new CustomArrayV4<T>(comp);
#endif

  tree = new RBTree<T>(comp);
  aUpdatesWait = std::unique_lock<std::mutex>(aUpdatesMutex);
  tUpdatesWait = std::unique_lock<std::mutex>(tUpdatesMutex);
  atUpdatesWait = std::unique_lock<std::mutex>(atUpdatesMutex);
  
  #ifndef TREE_ONLY
  pthread_create(&aThread, NULL, arrayThread<T>, this);
  #endif
  
  pthread_create(&tThread, NULL, treeThread<T>, this);
}

template <class T> SortedCollection<T>::~SortedCollection()
{
  Update<T> stop;
  stop.type = TYPE_STOP;
  treeUpdates.insert(stop);
  #ifndef TREE_ONLY
  arrayUpdates.insert(stop);
  #endif
  
  cout<<"Serviced from tree: \t"<<servicedFromTree<<endl;
  cout<<"Serviced from array: \t"<<servicedFromArray<<endl;
  cout<<"Times it wasn't ready: \t"<<numTimesWaitedOnLookup<<endl;
  
  
  #ifndef TREE_ONLY
  if(pthread_join(aThread, NULL) != 0)
    cout<<"Join failed."<<endl;
  #endif
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
  #ifndef TREE_ONLY
  arrayUpdates.insert(addition);
  #endif
}

template <class T> void SortedCollection<T>::del(int idx)
{
  numUpdates++;
  Update<T> deletion;
  deletion.type = TYPE_DELETE;
  deletion.idx = idx;
  treeUpdates.insert(deletion);
  #ifndef TREE_ONLY
  arrayUpdates.insert(deletion);
  #endif
}

template <class T> T SortedCollection<T>::lookup(int idx)
{
  bool ready = true;
  while(numTUpdates != numUpdates 
  #ifndef TREE_ONLY
    && !array->ready(numUpdates, idx)
  #endif
  ) 
  {
    if(ready)
    {
      ready = false;
      numTimesWaitedOnLookup++;
    }
    atReady.wait(atUpdatesWait);
  }
  
  #ifndef TREE_ONLY
  if(array->ready(numUpdates, idx))
  {
    servicedFromArray++;
    return array->lookup(idx);
  }
  #endif
    
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
  Update<T> u;
  while(true)
  {
    while(!arrayUpdates.remove(&u)) 
    {
      array->flush();
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
