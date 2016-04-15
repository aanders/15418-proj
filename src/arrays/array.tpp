#include "array.h"

template <class T> Array<T>::Array(bool (*c)(T a, T b))
{
  comp = c;
}
