// -*- coding:utf-8-with-signature -*-
// 最短経路を出力

//#include "tropical.h"
#include <Eigen/Dense>
#include <string>
#include <vector>
#include <iostream>

struct Edge
{
    char source;
    char target;

    float weight;

    constexpr Edge(char s, char t, float w): source(s), target(t), weight(w) { }
};

struct Path;
Path operator+ (const Path& lhs, const Path& rhs);

typedef Eigen::Matrix<Path, -1, -1> PathMatrix;

// 今度は乗法を連結にする.
struct Path
{
    constexpr static Edge INF = Edge('\0', '\0', 
                                     std::numeric_limits<float>::infinity() );

    // 実数でなくてもよい.
    // "" を INF とする
    std::string _path;

    float val;

    // Eigen 内部から呼び出される. 定義が必要。
    Path(float x) { 
        //throw std::logic_error("internal error"); 
        val = x;
    }

    Path(const Edge& v = INF) {
        if (v.source != '\0') 
            _path = std::string(1, v.source) + v.target;
        val = v.weight;
    }

    Path& operator += (const Path& rhs) { 
        Path path = *this + rhs; 
        _path = path._path;
        val = path.val;
        return *this;
    }

    static PathMatrix Identity(int size) {
        PathMatrix ret(size, size);
        for (int i = 0; i < size; ++i)
            ret(i, i) = Path(Edge('a' + i, 'a' + i, 0)); // こっちは 0
        return ret;
    }
};

bool operator == (const Path& lhs, const Path& rhs) {
    return lhs._path == rhs._path;
}

Path operator + (const Path& lhs, const Path& rhs) {
    if (lhs._path == "") return rhs;
    if (rhs._path == "") return lhs;
    return lhs.val <= rhs.val ? lhs : rhs;
}

Path operator * (const Path& lhs, const Path& rhs) {
    if (lhs._path == "") return lhs;
    if (rhs._path == "") return rhs;
    Path ret;
    ret._path = lhs._path + rhs._path;
    ret.val = lhs.val + rhs.val;
    return ret;
}

void print_matrix(const PathMatrix& m) {
    for (int i = 0; i < 4; ++i) {
        std::cout << "|";
        for (int j = 0; j < 4; ++j) {
            std::cout << "(" << m(i, j)._path << ", " << m(i, j).val << ")" <<
                         (j != 3 ? " " : "");
        }
        std::cout << "|\n";
    }
}

void shortest_path()
{
    PathMatrix id = Path::Identity(4);
    print_matrix(id);

    PathMatrix b(4, 4);
    b << Path(), Path(Edge('a', 'b', 2)), Path(Edge('a', 'c', 4)), Path(),
        Path(), Path(Edge('b', 'b', 0)), Path(Edge('b', 'c', 1)), Path(Edge('b', 'd', 9)),
        Path(), Path(), Path(), Path(Edge('c', 'd', 5)),
        Path(Edge('d', 'a', 3)), Path(), Path(), Path();

    PathMatrix c(4, 4);
    c = id + b;
    print_matrix(c);
    c = c * b;
    print_matrix(c);
    c = c * b;
    print_matrix(c);
}
