#pragma once

#include "../libcpplib/fuga.h"

namespace ClassLibrary1 {

public ref class FugaWrap
{
public:
    FugaWrap(System::String^ cs_str);

    ~FugaWrap();

    !FugaWrap();

    System::String^ get() ;

private:
    hoge::fuga* _ptr;
};

} // namespace ClassLibrary1

