// -*- mode:c++ -*-

#include <string>
#include <vector>
#include <algorithm>
#include "push-parse.tab.hh"
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
  std::vector<int>* m_vec;  // 所有する
  
public:
  EnumChecker(std::vector<int>* vec): m_vec(vec) { }
  
  virtual ~EnumChecker() {
    delete m_vec; m_vec = nullptr;
  }
  
  virtual bool check(int val) const {
    return std::find(m_vec->begin(), m_vec->end(), val) != m_vec->end();
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


// AST を兼ねる.
class Ident
{
  char* m_name;  // 所有する
  const YYLTYPE m_loc;
  
  int type; // -1 = undefined, 0 = variable, 1 = condition
  union {
    int value;
    Checker* checker;
  } u;
  
public:
  Ident(char* name, const YYLTYPE& loc):
                    m_name(name), m_loc(loc), type(-1) {
    assert( name );
    printf("name = %s\n", name );
  }

  virtual ~Ident() {
    if (type == 1)
      delete u.checker;
    free(m_name); m_name = nullptr;
  }

  const char* name() const { return m_name; }
  
  int get_val(int* error) const {
    if (type != 0) {
      *error = 1; return 0;
      //throw yy::parser::syntax_error(m_loc,
      //std::string("`") + m_name + "` is not a variable");
    }
    return u.value;
  }

  bool check_val(int val, int* error) const {
    if (type != 1) {
      *error =1; return 0;
      //throw yy::parser::syntax_error(m_loc,
      //std::string("`") + m_name + "` is not a condition");
    }
    return u.checker->check(val);
  }


  // @return 新たに挿入したときは true
  static bool var_update(char* name, int val, const YYLTYPE& , int* error );

  static void cond_define(char* name, Checker* check, const YYLTYPE&, int* error );
};


extern const Ident* find_ident(const std::string& name, const YYLTYPE& );

