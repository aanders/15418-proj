
#include <fstream>
#include <string>

#include "sortedCollection/sortedCollection.h"
#include "int_runner.h"
#define DEBUG

void IntRunner::runop(std::string op, std::string data,
    unsigned int trial)
{
  try {
    if (op.compare("insert") == 0)
    {
      int elt = std::stoi(data);
      if (trial == 0)
      {
        collection_.ins(elt);
      }
      else
      {
        tree_.insert(elt);
      }
#ifdef DEBUG
      std::cout << "Inserted " << elt << std::endl;
#endif
    }
    else if (op.compare("lookup") == 0)
    {
      std::size_t n = data.find(" ");
      if (n != string::npos) {
        int idx = std::stoi(data.substr(0,n));
        int ref = std::stoi(data.substr(n+1));
        int elt;
        if (trial == 0)
        {
          elt = collection_.lookup(idx);
        }
        else
        {
          RBNode<int> *node = tree_.lookupByIdx(idx);
          elt = (node != nullptr) ? node->val : 0;
        }
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
    else if (op.compare("delete") == 0)
    {
      int idx = std::stoi(data);
      if (trial == 0)
      {
        collection_.del(idx);
      }
      else
      {
        RBNode<int> *node = tree_.lookupByIdx(idx);
        if (node != nullptr) tree_.remove(node->val);
      }
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
  catch (const std::invalid_argument& e)
  {
    std::cerr << "ERROR: internal error: invalid data for instruction" << std::endl;
    std::cerr << "  at '" << op << " " << data << "'" << std::endl;
  }
}
