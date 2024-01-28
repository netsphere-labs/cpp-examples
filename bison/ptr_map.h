// -*- coding:utf-8-with-signature; mode:c++; tab-width:4; -*-

#ifndef PTR_MAP_H__
#define PTR_MAP_H__ 1

#include <map>
#include <type_traits>
#ifndef NDEBUG
  #include <stdio.h>
#endif


/**
 * 自動でdeleteするのはvalueだけ
 *
 * 使い方:
 *     ptr_map<string, Foo*> var;
 *     var.insert(make_pair("a", new Foo(...)));
 *
 * 実装メモ: 継承するのは誤り.
 */
template <typename _Key, typename _Tp, typename _Compare = std::less<_Key> >
class ptr_map
{
    static_assert(std::is_pointer<_Tp>::value,
                  "template parameter _Tp must be pointer type");

    typedef std::map<_Key, _Tp, _Compare> Container;
    Container container;

public:
    typedef typename Container::iterator       iterator;
    typedef typename Container::const_iterator const_iterator;
    typedef typename Container::key_type       key_type;
    // _Tp
    typedef typename Container::mapped_type    mapped_type;
    // std::pair<const _Key, _Tp>
    typedef typename Container::value_type     value_type;
    typedef typename Container::size_type      size_type;


    ~ptr_map() { clear(); }

    iterator begin() { return container.begin(); }
    const_iterator begin() const { return container.begin(); }

    iterator end() { return container.end(); }
    const_iterator end() const { return container.end(); }

#if __cplusplus >= 201103L
    const_iterator cbegin() const noexcept { return container.begin(); }

    const_iterator cend() const noexcept { return container.end(); }
#endif

    bool empty() const {
        return container.empty();
    }

    size_type size() const {
        return container.size();
    }

    const_iterator find( const key_type& k ) const {
        return container.find(k);
    }

    iterator find( const key_type& k ) {
        return container.find(k);
    }

/*
    // std::map .. 要素が見つからない場合は, コンテナ内に要素を生成して、それを
    //     返す. 後続の追加は、弾かれる。これはダメではないが、ちょっと微妙。
    // -> ポインタでは, 後続の追加で弾かれると単に leak するため, 挙動を変える.
    mapped_type& operator[](const key_type& k) {
        iterator i = container.find(k);
        if (i != container.end())
            return container[k]; // 参照を返す
        else
            return nullptr; // cannot bind non-const lvalue reference of type ‘S*&’ to a value of type ‘std::nullptr_t’   アカン..
    }
*/

    mapped_type& at(const key_type& k) {
        return container.at(k);
    }

    std::pair<iterator, bool> insert(const value_type& x) {
#ifndef NDEBUG
        if (!x.second)
            printf("warning: add NULL.\n");
#endif // !NDEBUG
        return container.insert(x);
    }


#if __cplusplus >= 201103L
    iterator erase(const_iterator it) {
        if (it == end())
            return end();
        delete it->second;
        return container.erase(it);
    }
#else
    void erase(iterator it) {
        if (it != end()) {
            delete it->second;
            container.erase(it);
        }
    }
#endif


    /**
     * @return  The number of elements erased.
     */
    size_type erase( const key_type& x ) {
        int r = 0;
        iterator i;
        while ( (i = find(x)) != end() ) {
            r++;
            erase(i);
        }
        return r;
    }


#if __cplusplus >= 201103L
    iterator erase(const_iterator __first, const_iterator __last) {
        iterator it = erase(__first);
        while (it != __last)
            it = erase(it);
        return it;
    }
#else
    void erase(iterator __first, iterator __last) {
        for (iterator i = __first; i != __last; i++)
            delete i->second;
        container.erase(__first, __last);
    }
#endif


    _Tp detach( iterator i ) {
        if (i == container.end())
            return nullptr;
        _Tp r = i->second;
        container.erase(i);
        return r;
    }

    _Tp detach( const key_type& name) {
        auto it = container.find(name);
        return detach(it);
    }


    void clear() {
        erase(begin(), end());
    }
};



#endif

// Local variables:
// tab-width: 4
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:
