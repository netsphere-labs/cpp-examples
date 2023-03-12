
//#include "mydll_c.h"
#include "mydll.h"
#include <assert.h>
using namespace std;

int foo = 10;

int func(const char* hoge) {
  return 100;
}

Base::~Base() {
  printf("Base::~Base()\n");
}

C::~C() {
  printf("C::~C()\n");
  assert(1);
}
