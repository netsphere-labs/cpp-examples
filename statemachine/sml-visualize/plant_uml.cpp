
// $ gcc -Wall -Wextra -I /home/hori/repos/sml/include/ -lstdc++ plant_uml.cpp
// C++17 でないとコンパイルできない。(C++14 は if constexpr がない.)

//
// Copyright (c) 2016-2020 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <algorithm>
#include <boost/sml.hpp>
#include <iostream>
#include <tuple>
#include <typeinfo>
#include <utility>
#include "sml-to-plantuml.h"

namespace sml = boost::sml;

struct e1 {};
struct e2 {};
struct e3 {};
struct e4 {};
struct e5 {};

struct guard {
  bool operator()() const { return true; }
} guard;

struct guard2 {
  bool operator()() const { return true; }
} guard2;

struct action {
  void operator()() {}
} action;

struct action2 {
  void operator()() {}
} action2;

void on_s1_entry_f() {}
void on_s2_exit_f() {}

struct on_s1_entry {
  auto operator()() { on_s1_entry_f(); }
} on_s1_entry;

struct on_s2_exit {
  auto operator()() { on_s2_exit_f(); }
} on_s2_exit;

struct sub_machine {
  auto operator()() const noexcept {
    using namespace sml;
    // clang-format off
    return make_transition_table(
       *"orth1"_s = X
      ,*"orth2"_s = X
    );
    // clang-format on
  }
};

struct plant_uml {
  auto operator()() const noexcept {
    using namespace sml;
    // clang-format off
    return make_transition_table(
       *"idle"_s + event<e1> = "s1"_s
      , "s1"_s + event<e2> [ !guard && guard2 ] / action = "s2"_s
      , "s1"_s + sml::on_entry<_> / on_s1_entry
      , "s2"_s + event<e3> [ guard || guard2 ] = "s1"_s
      , "s2"_s + sml::on_exit<_> / on_s2_exit
      , "s2"_s + event<e4> / action = state<sub_machine>
      , state<sub_machine> + event<e5> / (action, action2) = X
    );
    // clang-format on
  }
};

int main() {
  sml::sm<plant_uml> sm;
  sml::dump(sm, std::cout);
  return 0;
}

