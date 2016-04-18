#include <fstream>
#include <string>

#include "sortedCollection/sortedCollection.h"
#include "runner.h"

void Runner::run()
{
  std::string line;
  std::string op;

  while (std::getline(tracefile_, line))
  {
    std::size_t n = line.find(" ");
    if (n != string::npos)
    {
      runop(line.substr(0,n), line.substr(n+1));
    }
  }
}

