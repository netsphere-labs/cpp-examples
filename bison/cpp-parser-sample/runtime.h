// -*- mode:c++ -*-

#include <string>
#include <vector>
#include <algorithm>
#include "cpp-parse.tab.hh"
#ifndef NDEBUG
  #include <assert.h>
  #include <stdio.h>
#endif

class Checker
{
public:
  virtual ~Checker() { }
  
  // @return チェックに通らなかったら false
  virtual bool check(int val) const = 0;
};

class EnumChecker: public Checker
{
  const std::vector<int> m_vec;
  
public:
  EnumChecker(const std::vector<int>& vec): m_vec(vec) { }
  virtual ~EnumChecker() { }
  
  virtual bool check(int val) const {
    return std::find(m_vec.begin(), m_vec.end(), val) != m_vec.end();
  }
};

class RangeChecker: public Checker
{
  int m_begin, m_end;
public:  
  RangeChecker(int begin, int end): m_begin(begin), m_end(end) { }
  virtual ~RangeChecker() { }

  virtual bool check(int val) const {
    return val >= m_begin && val <= m_end;
  }
};


class Ident
{
  const std::string m_name;
  const yy::parser::location_type m_loc;
  
  int type; // -1 = undefined, 0 = variable, 1 = condition
  union {
    int value;
    Checker* checker;
  } u;
  
public:
  Ident(const std::string& name, const yy::parser::location_type& loc):
                    m_name(name), m_loc(loc), type(-1) {
    assert( name != "" );
    printf("name = %s\n", name.c_str());
  }

  virtual ~Ident() {
    if (type == 1)
      delete u.checker;
  }
  
  int get_val() const {
    if (type != 0) {
      throw yy::parser::syntax_error(m_loc,
                    std::string("`") + m_name + "` is not a variable");
    }
    return u.value;
  }

  bool check_val(int val) const {
    if (type != 1) {
      throw yy::parser::syntax_error(m_loc,
                    std::string("`") + m_name + "` is not a condition");
    }
    return u.checker->check(val);
  }


  static void var_update(const std::string& name, int val, const yy::parser::location_type& );

  static void cond_define(const std::string& name, Checker* check, const yy::parser::location_type& );
};


extern const Ident* find_ident(const std::string& name, const yy::parser::location_type& );

