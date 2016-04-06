#include "queue.cpp"
#include "sortedCollection.h"

template <class T> SortedCollection<T>::SortedCollection(
  bool (*c)(T a, T b)) 
{
  comp = c;
  set = std::set<T, bool(*)(T,T)>(comp);
}



template <class T> void SortedCollection<T>::ins(T t)
{

}
