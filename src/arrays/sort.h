#ifndef __sort_h__
#define __sort_h__

template <class T> void sort(T *tbs, int size, bool (*comp)(T a, T b),
     T* workspace)
{
  if(size < 2)
    return;
  
  int idx1 = 0;
  int idx2 = size / 2;
  
  T tmp;
  if(workspace == nullptr)
  {
    sort(tbs, idx2, comp, (T*) nullptr);
    sort(tbs + idx2, size - idx2, comp, (T*) nullptr);
    
    while(idx1 < idx2 && idx2 < size)
    {
      if(comp(tbs[idx2], tbs[idx1]))
      {
        tmp = tbs[idx2];
        for(int i = idx2; i > idx1; i--)
        {
          tbs[i] = tbs[i - 1];
        }
        tbs[idx1] = tmp;
        idx1++;
        idx2++;
      }
      else
      {
        idx1++;
      }
    }
  }
  else
  {
    sort(tbs, idx2, comp, workspace);
    sort(tbs + idx2, size - idx2, comp, workspace + idx2);
    
    int mainIdx = 0;
    int idx2Orig = idx2;
    while(idx1 < idx2Orig && idx2 < size)
    {
      if(comp(tbs[idx2], tbs[idx1]))
      {
        workspace[mainIdx] = tbs[idx2];
        idx2++;
      }
      else
      {
        workspace[mainIdx] = tbs[idx1];
        idx1++;
      }
      mainIdx++;
    }
    while(idx1 < idx2Orig)
    {
      workspace[mainIdx] = tbs[idx1];
      idx1++;
      mainIdx++;
    }
    while(idx2 < size)
    {
      workspace[mainIdx] = tbs[idx2];
      idx2++;
      mainIdx++;
    }
    
    for(int i = 0; i < size; i++)
    {
      tbs[i] = workspace[i];
    }
  }
  
}

#endif
