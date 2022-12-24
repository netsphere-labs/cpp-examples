#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "icontroller.h"
#include <boost/sml.hpp>

// インタフェイスの実装
class Controller : public IController {
public:
  Controller();
  virtual ~Controller();
  
    void startup();
    void shutdown();
    void do_start() const override;
    void do_stop() const override;

private:
  // ポインタにしないと, ヘッダで定義しないといけない.
  boost::sml::sm<struct FsmImpl>* fsm_;
};

#endif // CONTROLLER_H
