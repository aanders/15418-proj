#include <iostream>
#include "custom_v6.h"
#include "binSearch.h"

template <class T> CustomArrayV7<T>::CustomArrayV7(bool (*c)(T a, T b))
  : Array<T>(c)
{
  size = 0;
  allocated = 1;
  data = (T*) new char[allocated * sizeof(T)];
  iuhAndStart = 0;
  
  updates = new Updates<T>(c, V7_MAX_UPDATES);
  buffer = (T*) new char[(V7_MAX_UPDATES + 1) * sizeof(T)];
  
  currUpdates = updatesHandled = 0;
  
  updatesAcknowledged = 0;
  inc = 0;
  w1 = w2 = w3 = 0;
  
  #ifdef V7_DEBUG
  totalUpdates = maxFlushed = 0;
  timesFlushed[0] = timesFlushed[1] = 0;
  #endif
}

template <class T> CustomArrayV7<T>::~CustomArrayV7()
{
  delete[] buffer;
  
  #ifdef V7_DEBUG
  cout<<"Updates per flush: "<<totalUpdates / 
    (timesFlushed[0] + timesFlushed[1])<<endl;
  cout<<"Flushes due to readiness: "<<timesFlushed[0]<<endl;
  cout<<"Flushes due to fullness: "<<timesFlushed[1]<<endl;
  cout<<"Max flushed at once: "<<maxFlushed<<endl;
  #endif
}

template <class T> void CustomArrayV7<T>::ins(T a)
{
  long start = iuhAndStart & 0xFFFFFFFF;
  int i = updates->ins(a, data + start, size);
  int idx = updates->indices[i];
  
  long iuh = (iuhAndStart >> 32) & 0xFFFFFFFF;
  iuh++;
  iuhAndStart = (iuh << 32) | start;
  
  inc++;
  
  if(idx < size / 2)
  {
    //W1
    if(w1 > idx)
    {
      w1 = idx;
    }
    
    //W's 2 and 3
    if(w2 <= idx)
    {
      w2 = idx + 1;
    }
    else
    {
      w2++;
    }
    
    w3++;
    
    while(w2 >= size / 2 && w2 > 0)
    {
      w3 = w2;
      if(i == 0)
      {
        w2 = 0;
      }
      else
      {
        i--;
        w2 = updates->indices[i];
      }
    }
  }
  else
  {
    if(w3 > idx)
    {
      w3 = idx;
    }
  }
  updatesAcknowledged++;
  
  
  currUpdates++;
  if(updates->size == V7_MAX_UPDATES)
    flush(1);
  
  #ifdef V7_DEBUG
  totalUpdates++;
  #endif
}

template <class T> void CustomArrayV7<T>::del(int idx)
{
  int i = updates->del(idx);
  long iuh = (iuhAndStart >> 32) & 0xFFFFFFFF;
  iuh--;
  iuhAndStart = (iuh << 32) | (iuhAndStart & 0xFFFFFFFF);
  inc--;
  
  if(idx < size / 2)
  {
    //W1
    if(w1 > idx)
    {
      w1 = idx;
    }
    
    //W's 2 and 3
    if(w2 <= idx)
    {
      w2 = idx + 1;
    }
    else
    {
      w2--;
    }
    
    w3--;
    
    while(w3 < size / 2)
    {
      w2 = w3;
      if(i >= updates->size - 1)
      {
        w3 = size + inc;
      }
      else
      {
        i++;
        w3 = updates->indices[i];
      }
    }
  }
  else
  {
    if(w3 > idx)
    {
      w3 = idx;
    }
  }
  
  updatesAcknowledged++;
  
  currUpdates++;
  if(updates->size == V7_MAX_UPDATES)
    flush(1);
  
  #ifdef V7_DEBUG
  totalUpdates++;
  #endif
}

//here's hoping this gets optimized out
template <class T> void CustomArrayV7<T>::flush()
{
  flush(0);
}

template <class T> void 
  CustomArrayV7<T>::flush(int cause)
{
  if(updates->size == 0)
  {
    updatesHandled += currUpdates;
    currUpdates = 0;
    return;
  }
  
  #ifdef V7_DEBUG
  maxFlushed = (maxFlushed < updates->size ? updates->size : maxFlushed);
  timesFlushed[cause]++;
  #endif
  
  int insertsUnderHalf = 0;
  int updatesUnderHalf = 0;
  int insertsAfterHalf = 0;
  int updatesAfterHalf = 0;
  
  int heightUnder = 0;
  int heightAfter = 0;
  int allTimeLowUnder = 0;
  int allTimeLowAfter = 0;
  
  for(int i = 0; i < updates->size; i++)
  {
    if(updates->indices[i] < size / 2)
    {
      if(updates->types[i] == UPDATE_INSERT)
      {
        insertsUnderHalf++;
      }
      updatesUnderHalf++;
    }
    else
    {
      if(updates->types[i] == UPDATE_INSERT)
      {
        insertsAfterHalf++;
      }
      updatesAfterHalf++;
    }
  }
  
  int incUnderHalf = 2*insertsUnderHalf - updatesUnderHalf;
  int incAfterHalf = 2*insertsAfterHalf - updatesAfterHalf;
  
  T *start = data + (iuhAndStart & 0xFFFFFFFF);
  
  bool newlyAllocate = 
          (incUnderHalf > 0 && start - incUnderHalf <= data) ||
          (start + size + incAfterHalf >= data + allocated);
  
  T *newArr;
  T *newStart;
  if(newlyAllocate)
  {
    while(newlyAllocate)
    {
      allocated = allocated * V7_EXPAND_CONST;
      newlyAllocate = 
         (incUnderHalf > 0 && ((allocated - size) / 2) < incUnderHalf) ||
         (((allocated - size) / 2) + size + incAfterHalf >= allocated);
    }
    
    newArr = (T*) new char[allocated * sizeof(T)];
    long s = ((allocated - size) / 2) - incUnderHalf;
    newStart = newArr + s;
    
    int writeIdx = 0;
    int readIdx = 0;
    for(int i = 0; i < updates->size; i++)
    {
      while(writeIdx < updates->indices[i])
      {
        newStart[writeIdx] = start[readIdx];
        readIdx++;
        writeIdx++;
      }
      if(updates->types[i] == UPDATE_INSERT)
      {
        newStart[writeIdx] = updates->values[i];
        writeIdx++;
      }
      else
      {
        readIdx++;
      }
    }
    
    size += incUnderHalf + incAfterHalf;
    while(writeIdx < size)
    {
      newStart[writeIdx] = start[readIdx];
      writeIdx++;
      readIdx++;
    }
    
    delete[] data;
    data = newArr;
    iuhAndStart = s;
    w1 = size / 2;
    w2 = size / 2;
    w3 = size;
  }
  else
  {
    int writeIdx;
    int readIdx;
    int numUpdates;
    int numInserts;
    
    int bufferSize;
    int inBuffer;
    int bufferStart;
    
    //######## UNDER HALF: ########
    if(updatesUnderHalf > 0)
    {
      numUpdates = updatesUnderHalf;
      numInserts = insertsUnderHalf;
      bufferSize = numUpdates + 1;
      inBuffer = 0;
      bufferStart = 0;
      
      readIdx = 0;
      writeIdx = 0;
      newStart = start - incUnderHalf;
      
      while(readIdx < bufferSize && readIdx < size)
      {
        buffer[readIdx] = start[readIdx];
        readIdx++;
      }
      inBuffer = readIdx;
      
      for(int i = 0; i < numUpdates; i++)
      {
        while(writeIdx < updates->indices[i])
        {
          newStart[writeIdx] = buffer[bufferStart];
          writeIdx++;
          inBuffer--;
          bufferStart++;
          if(bufferStart == bufferSize)
          {
            bufferStart = 0;
          }
          if(readIdx < size)
          {
            buffer[(bufferStart + inBuffer) % bufferSize] = start[readIdx];
            inBuffer++;
            readIdx++;
          }
        }
        if(updates->types[i] == UPDATE_INSERT)
        {
          newStart[writeIdx] = updates->values[i];
          writeIdx++;
        }
        else
        {
          inBuffer--;
          bufferStart++;
          if(bufferStart == bufferSize)
          {
            bufferStart = 0;
          }
          if(readIdx < size)
          {
            buffer[(bufferStart + inBuffer) % bufferSize] = start[readIdx];
            inBuffer++;
            readIdx++;
          }
        }
      }
    }
    
    //######## AFTER HALF: #########
    if(updatesAfterHalf > 0)
    {
      numUpdates = updatesAfterHalf;
      numInserts = insertsAfterHalf;
      bufferSize = numUpdates + 1;
      inBuffer = 0;
      bufferStart = 0;

      writeIdx = updates->indices[updatesUnderHalf];
      readIdx = writeIdx - incUnderHalf;
      newStart = start - incUnderHalf;
      
      int idx = 0;
      while(idx < bufferSize && readIdx < size)
      {
        buffer[idx] = start[readIdx];
        readIdx++;
        idx++;
      }
      inBuffer = idx;

      for(int i = updatesUnderHalf; i < updates->size; i++)
      {
        while(writeIdx < updates->indices[i])
        {
          newStart[writeIdx] = buffer[bufferStart];
          writeIdx++;
          inBuffer--;
          bufferStart++;
          if(bufferStart == bufferSize)
          {
            bufferStart = 0;
          }
          if(readIdx >= 0)
          {
            buffer[(bufferStart + inBuffer) % bufferSize] = start[readIdx];
            inBuffer++;
            readIdx++;
          }
        }
        if(updates->types[i] == UPDATE_INSERT)
        {
          newStart[writeIdx] = updates->values[i];
          writeIdx++;
        }
        else
        {
          inBuffer--;
          bufferStart++;
          if(bufferStart == bufferSize)
          {
            bufferStart = 0;
          }
          if(readIdx >= 0)
          {
            buffer[(bufferStart + inBuffer) % bufferSize] = start[readIdx];
            inBuffer++;
            readIdx++;
          }
        }
      }
      
      while(inBuffer > 0)
      {
        newStart[writeIdx] = buffer[bufferStart];
        writeIdx++;
        inBuffer--;
        bufferStart++;
        if(bufferStart == bufferSize)
        {
          bufferStart = 0;
        }
        if(readIdx < size)
        {
          buffer[(bufferStart + inBuffer) % bufferSize] = start[readIdx];
          inBuffer++;
          readIdx++;
        }
      }
    }
    
    size += incUnderHalf + incAfterHalf;
    iuhAndStart = (iuhAndStart & 0xFFFFFFFF) - ((long) incUnderHalf);
    w1 = size / 2;
    w2 = size / 2;
    w3 = size;
  }
  
  updates->empty();
  
  updatesHandled += currUpdates;
  currUpdates = 0;
}

template <class T> T CustomArrayV7<T>::lookup(int idx)
{
  long iuh = (iuhAndStart >> 32) & 0xFFFFFFFF;
  long start = iuhAndStart & 0xFFFFFFFF;
  return data[start + idx - iuh];
}

template <class T> inline bool CustomArrayV7<T>::ready(int numUpdates,
                                                       int idx)
{
  int iuh = (iuhAndStart >> 32) & 0xFFFFFFFF;
  idx -= iuh;
  return numUpdates == updatesAcknowledged && 
          (idx < w1 || (idx >= w2 && idx < w3));
}
