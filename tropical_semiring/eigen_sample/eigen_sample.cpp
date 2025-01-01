
#include <iostream>
#include <Eigen/Dense>
#include <Eigen/Sparse>

// 密行列
void test_matrix()
{
    // `X` は任意サイズ, `d` ｈ double. `Eigen::Matrix<>` がテンプレートクラス.
    // 例えば 2 x 2 だと `Matrix2d`
    Eigen::MatrixXd m(2, 2);
    Eigen::MatrixXd b(2, 2);
    // 要素アクセスは `operator ()`. (行, 列) の順, 0始まり.
    m(0, 0) = 3;
    m(1, 0) = 2.5;
    m(0, 1) = -1;
    m(1, 1) = m(1, 0) + m(0, 1);

    // 次のように書いても代入できる
    b << 2, 3,   // 右が列 (見た目どおり)
        1, 4;
    std::cout << m << std::endl;
    std::cout << "m + b = \n" << m + b << std::endl;
}

// 疎行列
void test_sparse_matrix()
{
    typedef Eigen::Triplet<double> T;

    std::vector<T> tripletVec {
        T(0, 0, 0.2), T(0, 1, 0.5),   // (行, 列, 値) の組
        T(1, 0, 1.0), T(1, 1, 2.0) };

    // 疎な行列
    Eigen::SparseMatrix<double> M(2, 2); // rows, cols
    M.setFromTriplets(tripletVec.begin(), tripletVec.end());

    std::cout << M << std::endl;
/* 次が表示される
Nonzero entries:
(0.2,0) (1,1) (0.5,0) (2,1)

Outer pointers:
0 2  $

0.2 0.5
1 2
*/
}

// 行列の積
// A . B -> C で, Aの列数と B の行数が合っていれば積を計算できる. 
// 結果は, 行Aの行数 x 列Bの列数になる
void matrix_multiplication()
{
    Eigen::MatrixXd a(5, 3);
    Eigen::MatrixXd b(3, 4);

    a << 1, 2, 3,
         4, 5, 6,
         7, 8, 9, 
         10, 11, 12,
         13, 14, 15;
    b << 1, 1, 2, 3, 
         5, 8, 13, 21,
         34, 55, 89, 144;

    Eigen::MatrixXd c(5, 4);
    c = a * b;

    std::cout << c << std::endl;
}

int main()
{
    //test_matrix();
    //test_sparse_matrix();
    matrix_multiplication();

    return 0;
}

