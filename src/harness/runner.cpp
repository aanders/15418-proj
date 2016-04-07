
#include <fstream>
#include <string>

#include "sortedCollection/sortedCollection.h"
#include "runner.h"

template <class T>
Runner<T>::Runner(std::string tracefile)
{
  tracefile_.open(tracefile);
  if (!tracefile_.is_open())
  {
    // TODO: badbadbad
  }
}

template <class T>
Runner<T>::~Runner()
{
  tracefile_.close();
}

template <class T>
void Runner<T>::run()
{
  std::string line;
  std::string op;
  T data;

  while (std::getline(tracefile_, line))
  {
    int n = line.find(" ");
    runop_(line.substr(0,n), line.substr(n+1));
  }
}

template <class T>
void Runner<T>::runop_(std::string op, std::string data)
{
  if (op.compare("insert") == 0)
  {
    T elt;
    elt.loadFrom(data);
    collection_.ins(elt);
  }
  else if (op.compare("lookup") == 0)
  {
    collection_.lookup(std::stoi(data));
  }
  else if (op.compare("delete") == 0)
  {
    collection_.del(std::stoi(data));
  }
  else
  {
    // TODO: badbadbad
  }
}
