#include <iostream>
#include "custom_v5.h"
#include "binSearch.h"

template <class T> CustomArrayV5<T>::CustomArrayV5(bool (*c)(T a, T b))
  : Array<T>(c)
{
  size = 0;
  allocated = 1;
  data = (T*) new char[allocated * sizeof(T)];
  start = data;
  inserts = new CustomArrayV4<T>(c);
  numInserts = 0;
  
  #ifdef V5_DEBUG
  totalInserts = maxFlushed = 0;
  timesFlushed[0] = timesFlushed[1] = 0;
  #endif
}

#ifdef V5_DEBUG
template <class T> CustomArrayV5<T>::~CustomArrayV5()
{
  cout<<"Inserts per flush: "<<totalInserts / 
    (timesFlushed[0] + timesFlushed[1])<<endl;
  cout<<"Flushes due to readiness: "<<timesFlushed[0]<<endl;
  cout<<"Flushes due to deletions: "<<timesFlushed[1]<<endl;
  cout<<"Max flushed at once: "<<maxFlushed<<endl;
}
#endif

template <class T> void CustomArrayV5<T>::ins(T a)
{
  inserts->ins(a);
  numInserts++;
  
  #ifdef V5_DEBUG
  totalInserts++;
  #endif
}

//here's hoping this gets optimized out
template <class T> void CustomArrayV5<T>::flush()
{
  flush(0);
}

template <class T> void 
  CustomArrayV5<T>::flush(int cause)
{
  if(numInserts == 0)
    return;
  
  #ifdef V5_DEBUG
  maxFlushed = (maxFlushed < numInserts ? numInserts : maxFlushed);
  timesFlushed[cause]++;
  #endif
  
  int insertionIndices[numInserts];
  
  if(size == 0)
    for(int i = 0; i < numInserts; i++)
      insertionIndices[i] = i;
  
  int numUnderHalf = 0;
  
  if(size > 0)
  {
    for(int i = 0; i < numInserts; i++)
    {
      insertionIndices[i] = binarySearch(start, size, this->comp, 
        inserts->lookup(i)) + i;
      if(insertionIndices[i] < size / 2)
      {
        numUnderHalf++;
      }
    }
  }
  
  bool newlyAllocate = (numUnderHalf > 0 && start - numUnderHalf <= data) ||
           (start + size + numInserts - numUnderHalf >= data + allocated);
  
  if(newlyAllocate)
  {
    while(newlyAllocate)
    {
      allocated = allocated * V5_EXPAND_CONST;
      newlyAllocate = (numUnderHalf > 0 && (allocated / 3) < numUnderHalf) ||
          ((allocated / 3) + size + numInserts - numUnderHalf >= allocated);
    }
    T* newArr = (T*) new char[allocated * sizeof(T)];
    T* newStart = newArr + (allocated / 3) - numUnderHalf; // ok?
    
    int j = 0;
    for(int i = 0; i < numInserts; i++)
    {
      while(j < insertionIndices[i])
      {
        newStart[j] = start[j-i];
        j++;
      }
      newStart[j] = inserts->lookup(i);
      j++;
    }
    
    size += numInserts;
    while(j < size)
    {
      newStart[j] = start[j - numInserts];
      j++;
    }
    
    delete[] data;
    data = newArr;
    start = newStart;
  }
  else
  {
    start -= numUnderHalf;
    int j = 0;
    for(int i = 0; i < numUnderHalf; i++)
    {
      while(j < insertionIndices[i])
      {
        start[j] = start[j + numUnderHalf - i];
        j++;
      }
      start[j] = inserts->lookup(i);
      j++;
    }
    size += numInserts;
    j = size - 1;
    for(int i = numInserts - 1; i >= numUnderHalf; i--)
    {
      while(j > insertionIndices[i])
      {
        start[j] = start[j - i + numUnderHalf - 1];
        j--;
      }
      start[j] = inserts->lookup(i);
      j--;
    }
  }
  numInserts = 0;
  inserts->clearAll();
}

template <class T> void CustomArrayV5<T>::del(int idx)
{
  flush(1);
  
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
  
  if(size < allocated / (V5_EXPAND_CONST * V5_EXPAND_CONST))
  {
    allocated = allocated / V5_EXPAND_CONST;
    T *newArr = (T*) new char[allocated * sizeof(T)];
    T *newStart = newArr + (allocated / 3);
    
    for(int i = 0; i < size; i++)
    {
      newStart[i] = start[i];
    }
    
    delete[] data;
    data = newArr;
    start = newStart;
  }
}

template <class T> T CustomArrayV5<T>::lookup(int idx)
{
  return start[idx];
}

/*
template <class T> void CustomArray<T>::sortedChecker()
{
  bool ok = true;
  
  for(int i = 0; i < size - 1; i++)
  {
    if(this->comp(data[i+1], data[i]))
    {
      ok = false;
    }
    cout<<data[i]<<", ";
  }
  if(size > 0)
    cout<<data[size - 1]<<endl;
  else
    cout<<endl;
  
  if(!ok)
  {
    cout<<"Not ok bro.\n";
  }
}
*/
/*
template <class T> bool CustomArray<T>::lookupElt(T val)
{
  return this->comp(val, val);
}
*/
