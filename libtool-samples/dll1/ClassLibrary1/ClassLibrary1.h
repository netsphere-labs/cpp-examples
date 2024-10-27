#pragma once

#include "../libcpplib/fuga.h"

namespace ClassLibrary1 {

//template<typename T>
public ref class FugaWrap
{
public:
    FugaWrap(System::String^ cs_str);

    System::String^ get();

    int f() ;

    ~FugaWrap();
    !FugaWrap();


private:
    hoge::Fuga* _ptr;
};

} // namespace ClassLibrary1

