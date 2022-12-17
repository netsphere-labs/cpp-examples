// -*- coding:utf-8-with-signature; mode:c++; tab-width:4; -*-


#ifndef NONCOPYABLE_H__
#define NONCOPYABLE_H__ 1


// 使い方: private継承すること
class noncopyable
{
protected:
    noncopyable() { }
    ~noncopyable() {}

#if __cplusplus >= 201103L // C++11
    noncopyable( const noncopyable& ) = delete;
    noncopyable& operator = ( const noncopyable& ) = delete;
#else
private:
    noncopyable( const noncopyable& );
    noncopyable& operator = ( const noncopyable& );
#endif
};


#endif // !NONCOPYABLE_H__



// Local variables:
// tab-width: 4
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:
