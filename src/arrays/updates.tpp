#include <iostream>
#include "updates.h"
#include "binSearch.h"

template <class T> Updates<T>::Updates(bool (*c)(T a, T b), int ms)
{
  maxSize = ms;
  size = 0;
  firstOverHalf = 0;
  iuh = 0;
  comp = c;
  values = (T*) new char[ms  * sizeof(T)];
  types = new int[ms];
  indices = new int[ms];
}

template <class T> void Updates<T>::ins(T val, T* array, int asize)
{
  last = UPDATE_INSERT;
  if(size == maxSize)
  {
    std::cout<<"You tried to add too many updates!!!"<<std::endl;
    return;
  }
  int iuhCount = 0;
  
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
      {
        done = true;
      }
      else
      {
        insertionIndex++;
        if(indices[i] < asize / 2)
        {
          iuhCount++;
        }
      }
    }
    else
    {
      //if this is pre-delete
      if(insertionIndex < indices[i])
      {
        done = true;
      }
      else if(insertionIndex > indices[i])
      {
        insertionIndex--;
        if(indices[i] < asize / 2)
        {
          iuhCount--;
        }
      }
    }
  }
  
  for(int j = size; j > i; j--)
  {
    values[j] = values[j-1];
    indices[j] = indices[j-1] + 1; //note that this messes with delete indices
    //that's ok ONLY because of the way we actually execute these updates
    types[j] = types[j-1];
    if(indices[j] < asize / 2)
    {
      if(types[j] == UPDATE_INSERT)
      {
        iuhCount++;
      }
      else
      {
        iuhCount--;
      }
    }
  }
  values[i] = val;
  indices[i] = insertionIndex;
  types[i] = UPDATE_INSERT;
  if(indices[i] < asize / 2)
  {
    iuhCount++;
  }
  
  iuh = iuhCount;
  
  size++;
  
  while(firstOverHalf > 0 && indices[firstOverHalf - 1] >= asize / 2)
  {
    firstOverHalf--;
  }
  while(firstOverHalf < size && indices[firstOverHalf] < asize / 2)
  {
    firstOverHalf++;
  }
  
  return;
}

template <class T> void Updates<T>::del(int idx, int asize)
{
  last = UPDATE_DELETE;
  
  if(size == maxSize)
  {
    std::cout<<"You tried to add too many updates!!!"<<std::endl;
    return;
  }
  
  int iuhCount = 0;
  
  int i = 0;
  while(i < size && (indices[i] < idx || 
        (indices[i] == idx && types[i] == UPDATE_DELETE)))
  {
    if(indices[i] < asize / 2)
    {
      if(types[i] == UPDATE_INSERT)
      {
        iuhCount++;
      }
      else
      {
        iuhCount--;
      }
    }
    i++; //no need to adjust idx because it's pre-adjusted
  }
  
  if(i < size && indices[i] == idx && types[i] == UPDATE_INSERT)
  {
    for(int j = i; j < size - 1; j++)
    {
      indices[j] = indices[j+1] - 1;
      types[j] = types[j+1];
      values[j] = values[j+1];
      if(indices[j] < asize / 2)
      {
        if(types[j] == UPDATE_INSERT)
        {
          iuhCount++;
        }
        else
        {
          iuhCount--;
        }
      }
    }
    size--;
    
    iuh = iuhCount;
    
    while(firstOverHalf > size || 
          (firstOverHalf > 0 && indices[firstOverHalf - 1] >= asize / 2))
    {
      firstOverHalf--;
    }
    while(firstOverHalf < size && indices[firstOverHalf] < asize / 2)
    {
      firstOverHalf++;
    }
    return;
  }
  
  for(int j = size; j > i; j--)
  {
    values[j] = values[j-1];
    types[j] = types[j-1];
    indices[j] = indices[j-1] - 1; //note that this messes with delete indices
    //that's ok ONLY because of the way we actually execute these updates
    if(indices[j] < asize / 2)
    {
      if(types[j] == UPDATE_INSERT)
      {
        iuhCount++;
      }
      else
      {
        iuhCount--;
      }
    }
  }
  types[i] = UPDATE_DELETE;
  indices[i] = idx;
  if(indices[i] < asize / 2)
  {
    if(types[i] == UPDATE_INSERT)
    {
      iuhCount++;
    }
    else
    {
      iuhCount--;
    }
  }
  size++;
  
  iuh = iuhCount;
  
  while(firstOverHalf > 0 && indices[firstOverHalf - 1] >= asize / 2)
  {
    firstOverHalf--;
  }
  while(firstOverHalf < size && indices[firstOverHalf] < asize / 2)
  {
    firstOverHalf++;
  }
  
  return;
}

template <class T> void Updates<T>::empty()
{
  size = 0;
  firstOverHalf = 0;
  iuh = 0;
}
