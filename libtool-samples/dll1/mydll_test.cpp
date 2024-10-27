// -*- coding:utf-8-with-signature -*-

#include <stdio.h>
#include "libcpplib/fuga.h"

int main()
{
    hoge::Fuga* p = new hoge::Hage(L"日本語の文字列");
    printf("tarai = %d\n", p->f());
    
    delete p;
    return 0;
}
