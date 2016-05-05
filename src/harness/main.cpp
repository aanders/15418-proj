#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <unistd.h>

#include "int_runner.h"
#include "char_runner.h"
#include "memrunner.h"

using namespace std;

extern void runManualTests();

int parseInt(std::string s)
{
  try
  {
    return std::stoi(s);
  }
  catch (const std::invalid_argument& e)
  {
    throw MemRunner<int>::invalid_data();
  }
}

int main(int argc, char* argv[])
{
  bool USE_MEM_RUNNER = false;
  
  int c;
  while ((c = getopt(argc, argv, "m")) != -1)
  {
    switch (c)
    {
      case 'm':
        USE_MEM_RUNNER = true;
        break;
      default:
        std::cerr << "Error parsing command line arguments." << std::endl;
        break;
    }
  }
  std::cout << ((USE_MEM_RUNNER)? "Using Memrunner":"Using standard runner") << std::endl;
  if (optind < argc)
  {

    std::string tracefile(argv[optind]);

    std::ifstream f(tracefile);
    if (!f.is_open())
    {
      std::cerr << "Error: could not open file " << tracefile
          << ".  Aborting." << std::endl;
      return 1;
    }

    std::string header;
    if (!std::getline(f, header))
    {
      std::cerr << "Error: " << tracefile <<
          " has no content.  Aborting." << std::endl;
      return 1;
    }
    
    if (header.find("DATATYPE:") != 0)
    {
      std::cerr << "Error: " << tracefile <<
          " is not a valid trace file.  Aborting." << std::endl;
      return 1;
    }

    if (header.compare("DATATYPE:INT") == 0)
    {
      if (USE_MEM_RUNNER)
      {
        MemRunner<int> runner(f, [](int a, int b) { return a < b; });
        if (runner.loadTrace(&parseInt))
        {
          runner.run(3);
        }
        else
        {
          std::cerr << "Error: trace file did not load successfully.  Aborting" << std::endl;
          return 1;
        }
      }
      else // !USE_MEM_RUNNER
      {
        IntRunner runner(f);
        runner.run(); // automatically closes f
      }
    }
    else
    {
      std::cerr << "Error: unrecognized datatype declaration "
          << header << ".  Aborting." << std::endl;
      f.close();
      return 2;
    }
  }
  else
  {
    runManualTests();
  }
  return 0; 
}
