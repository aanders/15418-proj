
#include <fstream>
#include <string>

#include "sortedCollection/sortedCollection.h"
#include "char_runner.h"

void CharRunner::runop(std::string op, std::string data,
    unsigned int trial)
{
  if (op.compare("i") == 0)
  {
    char elt = data[0];
    collection_.ins(elt);
#ifdef DEBUG
    std::cout << "Inserted " << elt << std::endl;
#endif
  }
  else if (op.compare("l") == 0)
  {
    std::size_t n = data.find(" ");
    if (n != string::npos) {
      int idx = std::stoi(data.substr(0,n));
      char ref = data.substr(n+1).at(0);
      char elt = collection_.lookup(idx);
#ifdef DEBUG
      std::cout << "Lookup at index " << idx << std::endl;
      std::cout << "  -> expected " << ref << ", got " << elt << std::endl;
#endif
      if (ref != elt)
      {
        std::cerr << "ERROR: lookup failed: expected "
            << ref << ", got " << elt << std::endl;
      }
    }
  }
  else if (op.compare("d") == 0)
  {
    int idx = std::stoi(data);
    collection_.del(idx);
#ifdef DEBUG
    std::cout << "Deletion at index " << idx << std::endl;
#endif
  }
  else
  {
    std::cerr << "ERROR: internal error: unrecognized instruction" << std::endl;
    std::cerr << "  at '" << op << " " << data << "'" << std::endl;
  }
}

