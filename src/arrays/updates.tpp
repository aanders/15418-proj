#include <iostream>
#include "updates.h"
#include "binSearch.h"

template <class T> Updates<T>::Updates(bool (*c)(T a, T b), int ms)
{
  maxSize = ms;
  size = 0;
  comp = c;
  values = (T*) new char[ms  * sizeof(T)];
  types = new int[ms];
  indices = new int[ms];
}

template <class T> void Updates<T>::ins(T val, T* array, int asize)
{
  if(size == maxSize)
  {
    std::cout<<"You tried to add too many updates!!!"<<std::endl;
    return;
  }
  
  int insertionIndex = 0;
  if(asize > 0)
    insertionIndex = binarySearch(array, asize, comp, val);
  
  int i = -1;
  bool done = false;
  while(!done)
  {
    i++;
    if(i == size)
    {
      done = true;
    }
    else if(types[i] == UPDATE_INSERT)
    {
      //if val < values[i]
      if(comp(val, values[i]))
        done = true;
      else
        insertionIndex++;
    }
    else
    {
      //if this is pre-delete
      if(insertionIndex < indices[i])
        done = true;
      else if(insertionIndex > indices[i])
        insertionIndex--;
    }
  }
  
  for(int j = size; j > i; j--)
  {
    values[j] = values[j-1];
  }
  values[i] = val;
  for(int j = size; j > i; j--)
  {
    types[j] = types[j-1];
  }
  types[i] = UPDATE_INSERT;
  for(int j = size; j > i; j--)
  {
    indices[j] = indices[j-1] + 1; //note that this messes with delete indices
    //that's ok ONLY because of the way we actually execute these updates
  }
  indices[i] = insertionIndex;
  
  size++;
}

template <class T> void Updates<T>::del(int idx)
{
  if(size == maxSize)
  {
    std::cout<<"You tried to add too many updates!!!"<<std::endl;
    return;
  }
  
  int i = 0;
  //cout<<"Indices: ";
  while(i < size && (indices[i] < idx || 
        (indices[i] == idx && types[i] == UPDATE_DELETE)))
  {
    //cout<<indices[i]<<", ";
    i++; //no need to adjust idx because it's pre-adjusted
  }
  //cout<<endl;
  
  if(i < size && indices[i] == idx && types[i] == UPDATE_INSERT)
  {
    for(int j = i; j < size - 1; j++)
    {
      indices[j] = indices[j+1] - 1;
      types[j] = types[j+1];
      values[j] = values[j+1];
    }
    size--;
    return;
  }
  
  for(int j = size; j > i; j--)
  {
    values[j] = values[j-1];
  }
  for(int j = size; j > i; j--)
  {
    types[j] = types[j-1];
  }
  types[i] = UPDATE_DELETE;
  for(int j = size; j > i; j--)
  {
    indices[j] = indices[j-1] - 1; //note that this messes with delete indices
    //that's ok ONLY because of the way we actually execute these updates
  }
  indices[i] = idx;
  size++;
}

template <class T> void Updates<T>::empty()
{
  size = 0;
}
