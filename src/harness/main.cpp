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
  return 0; 
}
