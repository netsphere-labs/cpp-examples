// -*- coding:utf-8-with-signature -*-

#pragma once

#include <string>

// See https://gcc.gnu.org/wiki/Visibility
// OS とコンパイラの組み合わせで決まる
#if defined _WIN32 || defined __CYGWIN__
  // Windows
  #ifdef LIBCPPLIB_EXPORTS
    #ifdef __GNUC__
      #define DLL_PUBLIC __attribute__ ((dllexport))
    #else
      // Clang もこっちのようだ
      #define DLL_PUBLIC __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #else
    #ifdef __GNUC__
      #define DLL_PUBLIC __attribute__ ((dllimport))
    #else
      #define DLL_PUBLIC __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #endif
  #define DLL_LOCAL
#else
  // ELFフォーマット
  #if __GNUC__ >= 4
    #define DLL_PUBLIC __attribute__ ((visibility ("default")))
    #define DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define DLL_PUBLIC
    #define DLL_LOCAL
  #endif
#endif


namespace hoge {

#ifdef _WIN32
  typedef std::wstring UniString;
#else
  typedef std::u16string UniString;
#endif

  
class DLL_PUBLIC Fuga
{
public:
    // string があるので constexpr コンストラクタにできない
    Fuga(const UniString& s) noexcept;
    virtual ~Fuga() { }

    UniString get() const { return _str; }

    virtual int f() const = 0;
    
private:
    // primitive ではないメンバ
    UniString _str;
};


// 派生
class DLL_PUBLIC Hage : public Fuga
{
public:
    Hage(const UniString& s) noexcept: Fuga(s) { }

    // c++20: virtual メソッドを constexpr でオーバライド可能になった.
    // gcc 13 では, -std=c++20 を指定してもリンクエラー.
    // gcc 14 は OK.
#if __GNUC__ >= 14
    constexpr int f() const override;
#else
    int f() const override;
#endif
};
  
} // namespace hoge
