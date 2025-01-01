
// トロピカル半環を使った, グラフの最短経路の求め方

#include <iostream>
#include <Eigen/Dense>

///////////////////////////////////////////////////////////////////////
// Eigen は, 自作の型を要素にできる?

// 実数
struct Nat
{
    double val;

    // デフォルトコンストラクタが必要
    //Nat(): val(0) { }

    constexpr Nat(double v = double()): val(v) { }
};

constexpr Nat operator+ (const Nat& lhs, const Nat& rhs) {
    return Nat(lhs.val + rhs.val);
}

constexpr Nat operator* (const Nat& lhs, const Nat& rhs) {
    return Nat(lhs.val * rhs.val);
}

typedef Eigen::Matrix<Nat, -1, -1> MatrixNat;

// 自作の型で演算可能?  -> OK!
void test_nat()
{
    MatrixNat a(2, 2);
    a << Nat(1), Nat(2),
         Nat(3), Nat(4);

    Eigen::Vector2<Nat> b;
    b << Nat(3), Nat(4);

    MatrixNat c(2, 1);
    c = a * b;

    std::cout << c(0, 0).val << std::endl;
    std::cout << c(1, 0).val << std::endl;
}


///////////////////////////////////////////////////////////////////////
// トロピカル半環で, 最短経路の距離

#include "tropical.h"

constexpr bool operator== (const Tropical& lhs, const Tropical& rhs) {
    return lhs.val == rhs.val;
}

constexpr Tropical operator+ (const Tropical& lhs, const Tropical& rhs) {
    if (lhs.val == Tropical::INF) return rhs;
    if (rhs.val == Tropical::INF) return lhs;
    return Tropical(std::min(lhs.val, rhs.val));
}

// 乗法の単位元は 0
constexpr Tropical operator* (const Tropical& lhs, const Tropical& rhs) {
    return Tropical(lhs.val + rhs.val);
}


void print_matrix(const TroMatrix& m)
{
    for (int i = 0; i < 4; ++i) {
        std::cout << "|"; 
        for (int j = 0; j < 4; ++j) 
            std::cout << m(i, j).val << (j != 3 ? " " : "");
        std::cout << "|\n";
    }
}

void shortest_distance() 
{
    // 単位行列の形が違う
    TroMatrix id = Tropical::Identity(4);
    print_matrix(id);

    // グラフ
    TroMatrix b(4, 4);
    b << Tropical(), Tropical(2), Tropical(4), Tropical(),
        Tropical(), Tropical(0), Tropical(1), Tropical(9),
        Tropical(), Tropical(), Tropical(), Tropical(5),
        Tropical(3), Tropical(), Tropical(), Tropical();

    TroMatrix c(4, 4);
    // 単位行列と足すことで、その場所に留まれるようになる (対角成分が 0のため)。
    // 累乗の回数 "以内" に到達できる最短経路になる.
    c = id + b;
    print_matrix(c);
    c = c * b;
    print_matrix(c);
    c = c * b;
    print_matrix(c);
}


void shortest_path();

int main()
{
    shortest_distance();

    std::cout << "-----------------------\n";

    shortest_path();

    return 0;
}

