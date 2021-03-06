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
#include <cstring>
#include <sched.h>

/*
 * CHANGE THIS DEFINITION TO ALTER THE INTERNAL ARRAY VERSION
 */
#define ARRAY_VERSION 9
//#define TREE_ONLY
//#define BW_SUCK
//#define UPDATE_SPLIT

template <class T> void *arrayThread(void *sc)
{
  return ((SortedCollection<T>*) sc)->handleUpdatesArray();
}

template <class T> void *treeThread(void *sc)
{
  return ((SortedCollection<T>*) sc)->handleUpdatesTree();
}

template <class T> void *splitterThread(void *sc)
{
  return ((SortedCollection<T>*) sc)->updateSplitter();
}

static void *bandwidthProcess(void *argp);

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
  #ifdef BW_SUCK
  pthread_create(&bwThread, NULL, bandwidthProcess, this);
  #endif
  #ifdef UPDATE_SPLIT
  pthread_create(&usThread, NULL, splitterThread<T>, this);
  #endif
}

template <class T> SortedCollection<T>::~SortedCollection()
{
  Update<T> stop;
  stop.type = TYPE_STOP;
  #ifndef UPDATE_SPLIT
  treeUpdates.insert(stop);
  #ifndef TREE_ONLY
  arrayUpdates.insert(stop);
  #endif
  #endif
  #ifdef UPDATE_SPLIT
  allUpdates.insert(stop);
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
  #ifdef UPDATE_SPLIT
  if(pthread_join(usThread, NULL) != 0)
    cout<<"Join failed."<<endl;
  #endif
  
  #ifdef BW_SUCK
  pthread_cancel(bwThread);
  #endif
  
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
  #ifndef UPDATE_SPLIT
  treeUpdates.insert(addition);
  #ifndef TREE_ONLY
  arrayUpdates.insert(addition);
  #endif
  #endif
  #ifdef UPDATE_SPLIT
  allUpdates.insert(addition);
  #endif
}

template <class T> void SortedCollection<T>::del(int idx)
{
  numUpdates++;
  Update<T> deletion;
  deletion.type = TYPE_DELETE;
  deletion.idx = idx;
  #ifndef UPDATE_SPLIT
  treeUpdates.insert(deletion);
  #ifndef TREE_ONLY
  arrayUpdates.insert(deletion);
  #endif
  #endif
  #ifdef UPDATE_SPLIT
  allUpdates.insert(deletion);
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

template <class T> void *SortedCollection<T>::updateSplitter()
{
  Update<T> u;
  while(true)
  {
    while(!allUpdates.remove(&u)) {sched_yield();}
    treeUpdates.insert(u);
    arrayUpdates.insert(u);
    if(u.type == TYPE_STOP)
      break;
  }
  
  return NULL;
}

static void *bandwidthProcess(void *argp)
{
  int *bob = new int[500];
  int *joe = new int[500];
  bob[0] = 0;
  joe[0] = 0;
  volatile int hank = 0;
  while(true)
  {
    hank++;
  }
  while(true)
  {
    memcpy(bob, joe, 500 * sizeof(int));
    joe[0] = joe[0] + 1;
    memcpy(joe, bob, 500 * sizeof(int));
    bob[0] = bob[0] - 2;
  }
  
  return nullptr;
}
