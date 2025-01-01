// -*- coding:utf-8-with-signature -*-

#include <limits>
#include <Eigen/Dense>

struct Tropical;

// 先行して宣言
constexpr Tropical operator+ (const Tropical& lhs, const Tropical& rhs);

typedef Eigen::Matrix<Tropical, -1, -1> TroMatrix;

struct Tropical
{
    constexpr static double INF = std::numeric_limits<double>::infinity();

    // 実数 U {∞}
    double val;

    constexpr Tropical(double v = INF) : val(v) {}

    constexpr Tropical& operator+= (const Tropical& rhs) {
        val = (*this + rhs).val;
        return *this;
    }

    // 単位行列
    // 対角成分 = 0, 非対角成分 = INF
    static TroMatrix Identity(int size) {
        // 全部手で埋めると効率悪い
        //Eigen::Matrix<Tropical, -1, -1> ret = 
        //        Eigen::Matrix<Tropical, -1, -1>::Constant(size, size, Tropical());

        // どの値が使われる? -> 対角成分が勝手に 1.0 になる
        //Eigen::Matrix<Tropical, -1, -1>::Identity(size, size);

        // `Zero()` は固定サイズメソッド。動的サイズの行列には適用不可
        //  -> 値のデフォルトコンストラクタに任せればOK
        TroMatrix ret(size, size);
        // 対角成分だけ手で埋める
        for (int i = 0; i < size; ++i)
            ret(i, i) = Tropical(0);

        return ret;
    }
};
