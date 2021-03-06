#include <iostream>
#include "custom_v4.h"
#include "binSearch.h"

template <class T> CustomArrayV4<T>::CustomArrayV4(bool (*c)(T a, T b))
  : Array<T>(c)
{
  size = 0;
  allocated = 1;
  data = (T*) new char[allocated * sizeof(T)];
  start = data;
  updatesHandled = 0;
}

template <class T> void CustomArrayV4<T>::ins(T a)
{
  int insertionIndex = 0;
  if(size > 0)
    insertionIndex = binarySearch(start, size, this->comp, a);
  
  bool newlyAllocate = (size == allocated);
  if(insertionIndex < size / 2)
  {
    if(start == data)
      newlyAllocate = true;
  }
  else
  {
    if(start + size == data + allocated)
      newlyAllocate = true;
  }
  
  if(newlyAllocate)
  {
    allocated = allocated * V4_EXPAND_CONST;
    T* newArr = (T*) new char[allocated * sizeof(T)];
    T* newStart = newArr + ((allocated - size) / 2);
    
    for(int i = 0; i < insertionIndex; i++)
    {
      newStart[i] = start[i];
    }
    newStart[insertionIndex] = a;
    for(int i = insertionIndex; i < size; i++)
    {
      newStart[i + 1] = start[i];
    }
    
    size++;
    
    delete[] data;
    data = newArr;
    start = newStart;
  }
  else
  {
    //safe only because size < allocated
    if((insertionIndex < size / 2 && start > data) || 
       (start + size == data + allocated))
    {
      start--;
      for(int i = 0; i < insertionIndex; i++)
      {
        start[i] = start[i+1];
      }
      start[insertionIndex] = a;
    }
    else
    {
      for(int i = size; i > insertionIndex; i--)
      {
        start[i] = start[i-1];
      }
      start[insertionIndex] = a;
    }
    size++;
  }
  
  updatesHandled++;
}

template <class T> void CustomArrayV4<T>::del(int idx)
{
  if(idx < size / 2)
  {
    for(int i = idx; i > 0; i--)
    {
      start[i] = start[i-1];
    }
    start++;
  }
  else
  {
    for(int i = idx; i < size - 1; i++)
    {
      start[i] = start[i+1];
    }
  }
  
  size--;
  
  if(size < allocated / (V4_EXPAND_CONST * V4_EXPAND_CONST))
  {
    allocated = allocated / V4_EXPAND_CONST;
    T *newArr = (T*) new char[allocated * sizeof(T)];
    T *newStart = newArr + ((allocated - size) / 2);
    
    for(int i = 0; i < size; i++)
    {
      newStart[i] = start[i];
    }
    
    delete[] data;
    data = newArr;
    start = newStart;
  }
  
  updatesHandled++;
}

template <class T> T CustomArrayV4<T>::lookup(int idx)
{
  return start[idx];
}

template <class T> void CustomArrayV4<T>::flush() {}

template <class T> inline bool CustomArrayV4<T>::ready(int numUpdates, 
                                                      int idx)
{
  return numUpdates == updatesHandled;
}

template <class T> void CustomArrayV4<T>::clearAll()
{
  size = 0;
  start = data + ((allocated - size) / 2);
}
