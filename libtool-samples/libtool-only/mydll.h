
#include <string>

class Base
{
public:
  virtual ~Base();
};

class C: public Base
{
public:
  virtual ~C();
  
  std::string get_str();

  constexpr int hoge() {
    return 42;
  }
};
