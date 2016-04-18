#include <fstream>
#include <stdlib.h>

#include "int_runner.h"
#include "char_runner.h"

using namespace std;

extern void runManualTests();

int main(int argc, char* argv[])
{
  
  if (argc > 1)
  {
    std::ifstream f(argv[1]);
    IntRunner runner(f);
    runner.run(2);
  }
  else
  {
    runManualTests();
  }
  return 0; 
}
