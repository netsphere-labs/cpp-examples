// -*- mode:c++; coding:utf-8-with-signature -*-

#include "pch.h"
#include "fuga.h"

namespace hoge {

DLL_PUBLIC Fuga::Fuga(const UniString& s) noexcept : _str(s)
{
}

constexpr int tarai(int x, int y, int z) { 
    if (x <= y) return y;  // zを返すのは誤り. 
    return tarai(tarai(x - 1, y, z), 
               tarai(y - 1, z, x), 
               tarai(z - 1, x, y)); 
}

// error: 'constexpr' evaluation operation count exceeds limit of 33554432 (use '-fconstexpr-ops-limit=' to increase the limit)
#if __GNUC__ >= 14 || _MSC_VER
DLL_PUBLIC constexpr int Hage::f() const
#else
DLL_PUBLIC int Hage::f() const
#endif
{
#ifdef _MSC_VER 
    // error C2131: 1048576 /constexpr:steps のステップ制限を超える評価
    int r = tarai(15, 5, 0);
#else   
    constexpr int r = tarai(15, 5, 0);
#endif
    return r;
}
 
} // namespace hoge
