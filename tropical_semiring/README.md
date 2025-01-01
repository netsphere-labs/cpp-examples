# -*- coding:utf-8-with-signature -*-

# 行列の積で, グラフの最短経路を求める

有向グラフの最短経路 (始点から終点への経路のコストの和が最小) を求める方法がいくつかある。
 - Bellman-Ford法
   辺の重みが負でも動くが、重みに負の数がない場合は Dijkstra法のほうが速い.
   負の閉路があると正しい距離を求められないが, 負の閉路を検出できる。
 - Dijkstra法 (ダイクストラ法)
   負のコストの辺がない場合.
 - Floyd-Warshall法
   すべての2頂点間の最短距離を求める. ループは KIJ


### 全点対最短経路問題の計算量

Floyd-Warshall法について、行列積でも計算できる。
上手くやれば単純なループで求めるより (リソースの活用で) 性能が出る可能性がある。手書きの例; <a href="https://proc-cpuinfo.fixstars.com/2015/11/all-pair-shortest-path-html/">全点対間最短路を高速に求める</a>

計算量の下限  <a href="https://lealgorithm.blogspot.com/2017/08/blog-post_22.html">高速な全点対最短経路アルゴリズム</a>





## `eigen_sample/`

C++ 用の線形代数ライブラリ Eigen を使ってみる。

<a href="https://eigen.tuxfamily.org/index.php?title=Main_Page">Eigen is a C++ template library for linear algebra: matrices, vectors, numerical solvers, and related algorithms.</a>

ヘッダを `include` するだけでよく、非常に簡単に使える。

Web上の記事も多い。サンプルプログラムの例; <a href="https://gist.github.com/AtsushiSakai/5227370">行列演算ライブラリEigenの換算(Reduction)関連(平均,最大値,最小値，各行・列への演算)の関数サンプルプログラム</a>





## `tropical_semiring/`

この記事を C++ でなぞっただけ; <a href="https://qiita.com/lotz/items/094bffd77b24e37bf20e">動的計画法を実現する代数〜トロピカル演算でグラフの最短経路を計算する〜</a>

同様のネタ; <a href="https://codezine.jp/article/detail/9421">疎行列の計算を実装してグラフ理論をかじってみる</a>

<img src="graph.avif" width="320" />

記事中で出てくる半環 semi-ring は, いくつかの性質を満たす加法 (和) と乗法 (積) を持つ集合.
トロピカル半環は, 乗法を通常の加算, 加法を min() とする。加法の単位元 (零元) は∞, 乗法の単位元は 0. min-plus semiring とも呼ばれる。

max-plus の場合もトロピカル半環と呼ばれるらしい. 足し算を max(), 零元を -∞ にする。

このサンプルでは, 距離だけを求めるのと、経路も求めるのを作った。





## 発展

<a href="https://techblog.gmo-ap.jp/2023/11/06/rigs-and-diagrams/">半環と図</a> min-plus, max-plus だけではない。

発展 <a href="https://zenn.dev/lotz/articles/9c9ca0708b035b">閉半環を使ってグラフ上の最短距離を計算する！</a>



