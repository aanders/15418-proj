#include "queue.h"
#include "sortedCollection.h"
#include "arrays/array.h"
#include "arrays/vector_v1.h"
#include "arrays/custom_v1.h"
#include "arrays/custom_v2.h"
#include "arrays/custom_v3.h"
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
  array = new VectorArray<T>(comp);
  tree = new RBTree<T>(comp);
  aUpdatesWait = std::unique_lock<std::mutex>(aUpdatesMutex);
  tUpdatesWait = std::unique_lock<std::mutex>(tUpdatesMutex);
  
  pthread_t wat, morewat;
  pthread_create(&wat, NULL, arrayThread<T>, this);
  pthread_create(&morewat, NULL, treeThread<T>, this);
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
    aReady.wait(aUpdatesWait);
  }
  return array->lookup(idx);
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

template <class T> void *SortedCollection<T>::handleUpdatesTree()
{
  Update<T> u;
  while(true)
  {
    while(!treeUpdates.remove(&u))
    {
      tReady.notify_all();
    }
    if(u.type == TYPE_DELETE)
    {
      RBNode<T> *n = tree->lookupByIdx(u.idx);
      if (n != nullptr) tree->remove(n->val);
    }
    else if(u.type == TYPE_INSERT)
    {
      tree->insert(u.val);
    }

    numTUpdates++;
  }
  return NULL;
}
