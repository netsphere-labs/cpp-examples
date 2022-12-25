#include <iostream>
#include "controller.h"
#include <boost/sml.hpp>

// イベントはクラス.
struct ev_start {};
struct ev_stop {};

// auto で受けないと型が合わない => ヘッダで宣言できない.
auto ac_start = [](IController* ctrl) { ctrl->do_start(); };
auto ac_stop  = [](IController* ctrl) { ctrl->do_stop(); };

// C++11 user-defined literals を駆使している。
// "Idle"_s  => boost::ext::sml::v1_1_6::front::state<boost::ext::sml::v1_1_6::aux::string<char, 'I', 'd', 'l', 'e'> >
class idle; // 定義不要!
class driving;

// 状態遷移
struct FsmImpl {
  // 戻り値の型は auto でなければ, 大爆発する.
  //   => 宣言をヘッダに追い出せない. 一気に定義が必要.
  auto operator()() const noexcept {
    using namespace boost::sml;
// clang-format off
    return make_transition_table(
             // ガードを省略すると `always` 
             *state<idle>    + event<ev_start>  / ac_start    = state<driving>
            , state<driving> + event<ev_stop>   / ac_stop     = state<idle>
                               );
// clang-format on
  }
};

Controller::Controller() {
  // sml::sm のコンストラクタに渡した値が, action の引数として渡ってくる.
  fsm_ = new boost::sml::sm<FsmImpl>(static_cast<IController*>(this));
}

Controller::~Controller() {
  delete fsm_;
}
  
void Controller::startup() {
  // 直接, 別の状態に遷移するのではなく, かならずイベントを介す. Good.
  fsm_->process_event(ev_start{});
}

void Controller::shutdown() {
  fsm_->process_event(ev_stop{});
}

void Controller::do_start() const {
  std::cout << "do_start\n";
}

void Controller::do_stop() const {
  std::cout << "do_stop\n";
}
