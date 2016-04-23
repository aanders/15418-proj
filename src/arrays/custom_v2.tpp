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
  //cout<<numInsertions<<endl;
  
  if(numInsertions == 0)
    return;
  
  sort(insertions, numInsertions, this->comp, insertions+INSERT_BUFFER_SIZE);
  
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
      if(startIdx < size)
        startIdx += binarySearch(data + startIdx, size - startIdx, 
                              this->comp, insertions[i]);
      indices[i] = startIdx + i;
    }
  }
  
  T *newArr = data;
  bool aNewArr = false;
  
  if(size + numInsertions > allocated)
  {
    aNewArr = true;
    
    while(size + numInsertions > allocated)
    {
      allocated = allocated * 2;
    }
    newArr = new T[allocated];
    
    for(int i = 0; i < indices[0]; i++)
    {
      newArr[i] = data[i];
    }
  }
  
  int idx = numInsertions - 1;
  for(int i = size + numInsertions - 1; i >= indices[0]; i--)
  {
    if(i == indices[idx])
    {
      newArr[i] = insertions[idx];
      idx--;
    }
    else
    {
      newArr[i] = data[i - (idx + 1)];
    }
  }
  
  size += numInsertions;
  numInsertions = 0;
  
  if(aNewArr)
  {
    delete[] data;
    data = newArr;
  }
}

template <class T> void CustomArrayV2<T>::ins(T a)
{
  insertions[numInsertions] = a;
  numInsertions++;
  
  if(numInsertions == INSERT_BUFFER_SIZE)
  {
    //cout<<"buffer full:"<<endl;
    emptyInsertions();
  }
}

template <class T> void CustomArrayV2<T>::del(int idx)
{
  //cout<<"deleting:"<<endl;
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

template <class T> void CustomArrayV2<T>::sortedChecker(T *d, int s)
{
  bool ok = true;
  
  for(int i = 0; i < s - 1; i++)
  {
    if(this->comp(d[i+1], d[i]))
    {
      ok = false;
    }
    cout<<d[i]<<", ";
  }
  
  if(s > 0)
    cout<<d[s - 1]<<endl;
  else
    cout<<endl;
  
  if(!ok)
  {
    cout<<"Not ok bro.\n";
  }
}

template <class T> void CustomArrayV2<T>::flush() 
{
  //cout<<"flushed:"<<endl;
  emptyInsertions();
}

/*
template <class T> bool CustomArrayV2<T>::lookupElt(T val)
{
  return this->comp(val, val);
}
*/
