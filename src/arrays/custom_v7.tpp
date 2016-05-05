#include <iostream>
#include <mutex>
#include "custom_v7.h"
#include "binSearch.h"

#define V7_32B 0x00000000FFFFFFFF

template <class T> CustomArrayV7<T>::CustomArrayV7(bool (*c)(T a, T b))
  : Array<T>(c)
{
  size = 0;
  allocated = 1;
  data = (T*) new char[allocated * sizeof(T)];
  //iuhAndStart = 0;
  iuh = 0;
  start = data;
  
  updates = new Updates<T>(c, V7_MAX_UPDATES);
  buffer = (T*) new char[(V7_MAX_UPDATES + 1) * sizeof(T)];
  
  currUpdates = updatesHandled = 0;
  mainHasLock = false;
  
  updatesAcknowledged = 0;
  w0 = w1 = w2 = w3 = 0;
  
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
  //int64_t start = iuhAndStart & V7_32B;
  updates->ins(a, start, size);
  //int64_t iuh = updates->iuh;
  iuh = updates->iuh;
  //iuhAndStart = (iuh << 32) | start;
  
  if(updates->firstOverHalf == updates->size)
  {
    w3 = size;
  }
  else
  {
    w3 = updates->indices[updates->firstOverHalf];
  }
  
  if(updates->firstOverHalf == 0)
  {
    w2 = 0;
  }
  else
  {
    w2 = updates->indices[updates->firstOverHalf - 1] + 1;
  }
  
  if(updates->firstOverHalf == 0)
  {
    w1 = 0;
  }
  else
  {
    w1 = updates->indices[0];
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
  updates->del(idx, size);
  //int64_t iuh = updates->iuh;
  iuh = updates->iuh;
  //iuhAndStart = (iuh << 32) | (iuhAndStart & V7_32B);
  
  if(updates->firstOverHalf == updates->size)
  {
    w3 = size;
  }
  else
  {
    w3 = updates->indices[updates->firstOverHalf];
  }
  
  if(updates->firstOverHalf == 0)
  {
    w2 = 0;
  }
  else
  {
    w2 = updates->indices[updates->firstOverHalf - 1] + 1;
  }
  
  if(updates->firstOverHalf == 0)
  {
    w1 = 0;
  }
  else
  {
    w1 = updates->indices[0];
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
  
  //T *start = data + (iuhAndStart & V7_32B);
  
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
    int s = ((allocated - size) / 2) - incUnderHalf;
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
    
    handyLock.lock();
    delete[] data;
    data = newArr;
    start = newStart;
    iuh = 0;
    w0 = 0;
    w1 = size / 2;
    w2 = size / 2;
    w3 = size;
    handyLock.unlock();
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
      
      writeIdx = 0;
      readIdx = 0;
      newStart = start - incUnderHalf;
      
      int i = 0;
      while(i < bufferSize && readIdx < size)
      {
        buffer[i] = start[readIdx];
        readIdx++;
        i++;
      }
      inBuffer = i;
      
      handyLock.lock();
      w1 = 0;
      handyLock.unlock();
      
      for(int i = 0; i < numUpdates; i++)
      {
        while(writeIdx < updates->indices[i])
        {
          newStart[writeIdx] = buffer[bufferStart];
          writeIdx++;
          w0++;
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
          w0++;
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
          w3++; //MOVE THE BARRIER BACK
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
          w3++;
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
      
      while(inBuffer > 0)
      {
        newStart[writeIdx] = buffer[bufferStart];
        writeIdx++;
        w3++;
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
    handyLock.lock();
    //int64_t iuh = iuhAndStart >> 32;
    //iuhAndStart = (iuhAndStart & V7_32B) - ((int64_t) incUnderHalf);
    start = newStart;
    iuh = 0;
    w0 = 0;
    w1 = size / 2;
    w2 = size / 2;
    w3 = size;
    handyLock.unlock();
  }
  
  updates->empty();
  
  updatesHandled += currUpdates;
  currUpdates = 0;
}

template <class T> T CustomArrayV7<T>::lookup(int idx)
{
  //int64_t start = iuhAndStart & V7_32B;
  if(mainHasLock)
  {
    //int64_t iuh = iuhAndStart >> 32;
    if(!(idx < w1)) //either idx < w0 or w2 <= idx < w3
    {
      idx -= iuh;
    }
  }
  T ret = start[idx];
  if(mainHasLock)
  {
    handyLock.unlock();
    mainHasLock = false;
  }
  return ret;
}

template <class T> inline bool CustomArrayV7<T>::ready(int numUpdates,
                                                       int idx)
{
  if(numUpdates != updatesAcknowledged)
  {
    return false;
  }
  if(numUpdates == updatesHandled)
  {
    return true;
  }
  
  if(!mainHasLock)
  {
    handyLock.lock();
    mainHasLock = true;
  }
  
  bool ret = (idx < w1) || 
             (idx < w0) || //even though we use index offsetting
             (idx >= w2 && idx < w3);
  if(!ret)
  {
    handyLock.unlock();
    mainHasLock = false;
  }
  return ret;
}
