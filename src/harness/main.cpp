#include <iostream>
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
  SortedCollection<char> sc(&charComp);
  sc.ins(5);
  cout<<sc.lookup(0)<<endl;
  return 0; 
}
