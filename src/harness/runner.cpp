#include <iostream>
#include <fstream>
#include <string>

#include "CycleTimer.h"

#include "sortedCollection/sortedCollection.h"
#include "runner.h"

void Runner::run(unsigned int trials)
{
  std::string line;
  std::string op;

  for (unsigned int t = 0; t < trials; t++)
  {
    tracefile_.clear();
    tracefile_.seekg(0, ios::beg);
    double start = CycleTimer::currentSeconds();
    while (std::getline(tracefile_, line))
    {
      std::size_t n = line.find(" ");
      if (n != string::npos)
      {
        runop(line.substr(0,n), line.substr(n+1), t);
      }
    }
    double end = CycleTimer::currentSeconds();
    std::cout << "Trial " << t << " took " << end-start
        << " seconds to complete" << std::endl;
  }
}

