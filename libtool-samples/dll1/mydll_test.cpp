﻿// -*- coding:utf-8-with-signature -*-

#include <stdio.h>
#include "libcpplib/fuga.h"
#include <codecvt> // C++17 で非推奨, C++26 で廃止
#include <locale>

int main()
{
    // wstring_convert は C++17 で非推奨, C++26 で廃止
    std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> cvt;
    
    std::u16string d = cvt.from_bytes("日本語の文字列");
    hoge::Fuga* p = new hoge::Hage(d);
    printf("tarai = %d\n", p->f());
    
    delete p;
    return 0;
}
