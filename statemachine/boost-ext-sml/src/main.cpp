#include "controller.h"

int main() {
  Controller ctrl;
  ctrl.startup();   // ループではない. 1ステップのみ動かす
  ctrl.shutdown();
  
  return 0;
}
