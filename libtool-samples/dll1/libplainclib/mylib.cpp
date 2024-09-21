// -*- coding:utf-8-with-signature -*-

#include "pch.h"
#include "mylib.h"
#include <string>
#include <map>
using namespace std;

struct B {
    typedef pmr::map<string, string> Container;
    Container kv;
};

B* make_b() {
    return new B();
}

// 挿入
void b_insert_str(B* kv, const char* key, const char* value)
{
    // insert() だと更新されない
    kv->kv[string(key)] = value;
}

const char* b_find(B* kv, const char* s)
{
    B::Container::const_iterator i = kv->kv.find(s) ;
    return i == kv->kv.end() ? nullptr : i->second.c_str();
}

void b_free(B* kv) {
    delete kv;
}
