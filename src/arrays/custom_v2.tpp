#include <iostream>
#include "sort.h"
#include "custom_v2.h"
#include "binSearch.h"

template <class T> CustomArrayV2<T>::CustomArrayV2(bool (*c)(T a, T b)) 
  : Array<T>(c)
{
  size = 0;
  allocated = 1;
  data = new T[allocated];
  
  numInsertions = 0;
}


template <class T> void CustomArrayV2<T>::emptyInsertions()
{
  //cout<<"Before:\n";
  //printData(data, size);
  
  if(numInsertions == 0)
    return;
  
  sort(insertions, numInsertions, this->comp);
  //cout<<"Sorted insertions:\n";
  //printData(insertions, numInsertions); 
  
  int startIdx = 0;
  int indices[numInsertions];
  
  if(size == 0)
  {
    for(int i = 0; i < numInsertions; i++)
      indices[i] = i;
  }
  else
  {
    for(int i = 0; i < numInsertions; i++)
    {
      startIdx = binarySearch(data, size, 
                              this->comp, insertions[i]);
      indices[i] = startIdx + i;
    }
  }
  
  if(size + numInsertions > allocated)
  {
    //cout<<"Neither here\n";
    while(size + numInsertions > allocated)
    {
      allocated = allocated * 2;
    }
    T *newArr = new T[allocated];
    int offset = 0;
    for(int i = 0; i < size + numInsertions; i++)
    {
      if(offset < numInsertions && i == indices[offset])
      {
        newArr[i] = insertions[offset];
        offset++;
      }
      else
      {
        newArr[i] = data[i - offset];
      }
    }
    
    delete[] data;
    data = newArr;
  }
  else
  {
    //cout<<"Nor there.\n";
    int idx = numInsertions - 1;
    for(int i = size + numInsertions - 1; i >= indices[0]; i--)
    {
      if(i == indices[idx])
      {
        data[i] = insertions[idx];
        idx--;
      }
      else
      {
        data[i] = data[i - (idx + 1)];
      }
    }
  }
  
  size += numInsertions;
  numInsertions = 0;
  //cout<<"After:\n";
  //printData(data, size);
}

template <class T> void CustomArrayV2<T>::ins(T a)
{
  insertions[numInsertions] = a;
  numInsertions++;
  
  if(numInsertions == INSERT_BUFFER_SIZE)
  {
    emptyInsertions();
  }
}

template <class T> void CustomArrayV2<T>::del(int idx)
{
  emptyInsertions();
  
  for(int i = idx; i < size - 1; i++)
  {
    data[i] = data[i+1];
  }
  
  size--;
  
  if(size < allocated / 4)
  {
    T *newArr = new T[allocated / 2];
    allocated = allocated / 2;
    for(int i = 0; i < size; i++)
    {
      newArr[i] = data[i];
    }
    
    delete[] data;
    data = newArr;
  }
}

template <class T> T CustomArrayV2<T>::lookup(int idx)
{
  emptyInsertions();
  return data[idx];
}

template <class T> void CustomArrayV2<T>::printData(T *d, int s)
{
  for(int i = 0; i < s; i++)
  {
    cout<<d[i];
    if(i < s - 1)
      cout<<", ";
  }
  cout<<endl;
}

/*
template <class T> bool CustomArrayV2<T>::lookupElt(T val)
{
  return this->comp(val, val);
}
*/
