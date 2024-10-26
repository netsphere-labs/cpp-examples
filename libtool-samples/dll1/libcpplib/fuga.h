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

class DLL_PUBLIC fuga
{
public:
    // string があるので constexpr コンストラクタにできない
    fuga(const UniString& s) noexcept;

    UniString get() const { return _str; }

private:    
    UniString _str;
};

} // namespace hoge
