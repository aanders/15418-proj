#include "sortedCollection.h"

template <class T> SortedCollection<T>::SortedCollection(int (*c)(T a, T b))
{
  comp = c;
}
