// -*- coding:utf-8-with-signature -*-

#include "pch.h"

#include "ClassLibrary1.h"
#include <msclr/marshal_cppstd.h>

using namespace System;

/*
C# -> C++ 文字列変換
サポートされる組み合わせ:
See https://learn.microsoft.com/ja-jp/cpp/dotnet/overview-of-marshaling-in-cpp?view=msvc-170
  <marshal.h>          const char*, const wchar_t*
  <marshal_windows.h>  BSTR, bstr_t
  <marshal_cppstd.h>   std::string, std::wstring
  <marshal_atl.h>      CStringT<char>, CStringT<wchar_t>, CComBSTR
*/

namespace ClassLibrary1 {

// コンストラクタ
//template <class T>
FugaWrap::FugaWrap(String^ cs_str)
{
    Console::WriteLine("enter FugaWrap::FugaWrap()");

    std::wstring cpp_str = msclr::interop::marshal_as<std::wstring>(cs_str);
    _ptr = new hoge::Hage(cpp_str);
}

/*
C# 側は次のようなコードになっている:
@param disposing true = Dispose() が呼び出された場合.
protected override void Dispose(bool disposing) {
    if (!_disposed) {
        if (disposing) {
            // TODO: Dispose managed resources here.
            //       アンマネージリソースも解放すること
            ここで C++/CLI オブジェクトの ~dtor() が呼び出される
        }
        else {  // GC が呼び出す. 呼び出されるとは限らない
            // TODO: Free unmanaged resources *ONLY* here.
            ここで C++/CLI オブジェクトの ~finalizer() が呼び出される
        }

        // Note disposing has been done.
        _disposed = true;

        // Call the base class implementation.
        base.Dispose(disposing);
    }
}
*/

// デストラクタ.
// using() から抜けるときに呼び出されるのはこっち. マネージとアンマネージの両方を解放
//template <class T>
FugaWrap::~FugaWrap()
{
    Console::WriteLine("enter FugaWrap::~FugaWrap()");
    // 1. マネージリソースはここで解放

    // 2. ファイナライザを陽に呼び出す
    this->!FugaWrap();
}

// ファイナライザ
// GC が発生するときに呼び出される「ことがある」. 
// TODO: アンマネージ「だけ」解放。マネージはGCが解放
//template <class T>
FugaWrap::!FugaWrap()
{
    Console::WriteLine("enter FugaWrap::!FugaWrap()");

    // アンマネージ
    delete _ptr; _ptr = nullptr;
}

//template <class T>
String^ FugaWrap::get()
{
    // C++ -> C#
    String^ ret = msclr::interop::marshal_as<String^>(_ptr->get());
    return ret;
}

int FugaWrap::f()
{
    return _ptr->f();
}

} // namespace ClassLibrary1
