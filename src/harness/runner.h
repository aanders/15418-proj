

#include <fstream>
#include <string>

#include "sortedCollection/sortedCollection.h"


template <class T>
class Runner
{
  private:
    SortedCollection<T> collection_;
    std::ifstream tracefile_;

    void runop_(std::string op, std::string data);

  public:

    Runner(std::string tracefile);
    ~Runner();
    
    void run();
};
