#include <iostream>
#include <stdlib.h>
#include "sortedCollection/sortedCollection.h"

using namespace std;

bool intComp(int a, int b)
{
  return a < b;
}

bool charComp(char a, char b)
{
  return a < b;
}

int main(void)
{
  const int size = 15;
  int additions[size] = {5,4,5,2,9,
                         1, -3, 9, 22, 490,
                         11, 12, -7345353, 12, 18};
  
  SortedCollection<int> sc(&intComp);

  for(int i = 0; i < size; i++)
  {
    sc.ins(additions[i]);
    for(int j = 0; j < i + 1; j++)
    {
      int res = sc.lookup(j);
      cout<<res<<endl;
    }
    cout<<endl;
  }
  
  /*
  int tests[size] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15};
  int refsol[size] = {1,1,0,1,1,0,0,0,1,0,1,1,0,0,0};
  
  for(int i = 0; i < size; i++)
  {
    if (sc.lookupElt(tests[i]) && (refsol[i] == 1))
    {
      cout<<"RBT test "<<i<<" passed! (found element)"<<endl;
    }
    else if (!sc.lookupElt(tests[i]) && (refsol[i] == 0))
    {
      cout<<"RBT test "<<i<<" passed! (element doesn't exist)"<<endl;
    }
    else
    {
      cout<<"RBT test "<<i<<" failed :("<<endl;
    }
  }
  */
  return 0; 
}
