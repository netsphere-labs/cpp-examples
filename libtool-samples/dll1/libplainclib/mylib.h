// -*- coding:utf-8-with-signature -*-

#pragma once

// 公開設定は .def ファイルで行う

// /mingw64/bin/dlltool.exe
// $ dlltool--export - all --output - def kaijyou.def kaijyou.o

extern "C" {

struct B;

extern B* make_b();

// 挿入
extern void b_insert_str(B*, const char* key, const char* value);

extern const char* b_find(B*, const char* s);

extern void b_free(B*);

}

