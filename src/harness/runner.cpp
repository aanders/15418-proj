#include <fstream>
#include <string>

#include "sortedCollection/sortedCollection.h"
#include "runner.h"

void Runner::run(unsigned int trials)
{
  std::string line;
  std::string op;

  for (unsigned int t = 0; t < trials; t++)
  {
    tracefile_.seekg(0, ios::beg);
    while (std::getline(tracefile_, line))
    {
      std::size_t n = line.find(" ");
      if (n != string::npos)
      {
        runop(line.substr(0,n), line.substr(n+1), t);
      }
    }
  }
}

