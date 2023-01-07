# UML StateMachines

UML 1.5 では statechart diagram という名前だった. UML2 で StateMachines パッケージに名前変更。

## サンプルプログラム

### `qt-trafficlight-handwritten/`

状態遷移のサンプル。Qt5/Qt6 両対応. これをベースにした; <a href="https://doc.qt.io/qt-6/qtstatemachine-statemachine-trafficlight-example.html">Traffic Light Example | Qt State Machine 6.4.1</a>

状態 (state) が X のときにイベント E が発生すると, 次の状態 Y に遷移する、と定義していく。状態は `QState` クラスインスタンスとする。`addTransition()` メソッドでイベントと遷移先を登録する。

このサンプルでは, 黄色信号は赤に行くこともあれば青に行くこともある。遷移させるイベントはいずれも timeout. イベントが同じなのに行き先が二つ以上の場合は、状態を二つに分けてやる。

Qt の状態遷移は signal &amp; slot の仕組みを活用しており、タイムアウトや他のイベント (ボタンを押したなど) も統一的に扱える。他方、Qt イベントループへの登録が前提になっており、かなり重い処理になっている。

状態遷移図を手書きで実装すべきではない。穴が開いているかどうかまるで分からない。Qt は SCXML をインポートしてそのまま実行できるので、GUI で状態遷移図を描くとよい。



### `qt-trafficlight-scxml-dynamic/`

Qt5/Qt6 両対応。SCXML ファイルをそのまま利用する信号機サンプル。Based on https://doc.qt.io/qt-6/qtscxml-trafficlight-widgets-dynamic-example.html

SCXML は状態遷移図の交換用フォーマット。<a href="https://alexzhornyak.github.io/SCXML-tutorial/">SCXML-tutorial | SCXML illustrated examples and tutorials</a>. Qt Creator は GUI で編集できる。

このサンプルは, SCXML をコンパイルせずに、そのまま読み込む。`QScxmlStateMachine` クラスインスタンスが SCXML の内容を保持する。`connectToState()` メソッドでコールバック関数を登録する。コールバック関数は, 次の宣言のように, `bool` 型の引数を持つこと。

```c++
  void mySlot(bool active);
```

SCXML は <code>&lt;send event="startGoingGreen" delay="3s" /&gt;</code> のようにタイムアウトをこの中で書けるため、インタプリタは必然的にイベントループを必要とする。





### `boost-ext-sml/`

Based on https://github.com/ubeh/fsm_examples/

[Boost::ext].SML は軽量な状態機械ライブラリでメジャー。名前が紛らわしいが、Boost の一部ではなく, Boost には依存しない。SML では state はクラスではない。コードを書きたい場合は, action でコールバックさせるぐらい。




### `sml-visualize/`

SML の状態遷移のコード定義を PlantUML のテキストとして出力する。これそのまま; https://github.com/boost-ext/sml/blob/master/example/plant_uml.cpp

コンパイルには C++17 が必要 (C++14 には `if constexpr` がない). Template metaprogramming (TMP) の権化みたいなコードになっている。正直, 可変長テンプレート引数は理解の範囲を超える.





## 有限状態機械 (Finite State Machine; FSM) ライブラリ

Qt の `QStateMachine` は, 1クラスのなかで状態を管理したいといったような, 軽い用途には使いにくい。状態機械ライブラリを探した。山のようにあるが、まぁまぁ人気のものを挙げる。

UML2 では, <i>behavior (behavioural) state machines</i> and <i>protocol state machines</i> という区分を挙げている。求められる機能性の違いは微妙だが、前者は状態が動名詞になりやすい。State が内部状態や振る舞いを持つため, クラスとして定義できたほうがよいようだ。



### 軽量ライブラリ

#### ☆ <a href="https://boost-ext.github.io/sml/">[Boost::ext].SML</a>
<i>Boost.Statechart</i> はパフォーマンスが悪く, Boost Meta State Machine (MSM) はコンパイルの時間・メモリを大量に食う。SML は, パフォーマンスは MSM 並で、コンパイルが多少は軽くなっている。
しかし, C++ の超絶技巧を駆使しており、エラーが出た場合など挙動がさっぱり分からないのが傷。

#### ▲ <a href="https://github.com/erikzenker/hsm/">Hana State Machine (HSM)</a> 
[Boost::ext].SML と Boost.MSM の実装の仕方を <i>Boost.Hana</i> で再実装し, 独自のメタプログラミングコードの大きさを縮小。パフォーマンス傾向は MSM に似ている。悪くない。しかし, 黒魔術であることには変わらないので, SML を使えばよい。

#### ▲ <a href="https://github.com/digint/tinyfsm/">TinyFSM</a>
大きさが小さい。リソース制約がキツい用途では, よさそう。

イベントがクラス. <code>tinyfsm::Event</code> から派生させる。状態もクラス. 継承関係がちょっと変わっている。<code>tinyfsm::Fsm&lt;&gt;</code> クラスを派生させて状態機械クラスを定義し、そこから各状態クラスを派生させる。

<code>状態機械クラス#dispatch(<var>event</var>)</code> を呼び出すと状態クラスの <code>react(<var>eventType&amp;</var> )</code> メソッドが呼び出される。そのなかで <code>transit&lt;<var>stateClass</var>&gt;()</code> で遷移させる。
複数の状態機械を設ける場合は <code>tinyfsm::FsmList&lt;StateClass, ...&gt;</code> を使う。<code>template dispatch&lt;eventType&gt;(event)</code> で全状態機械にイベントを配信できる。

▲<code>transit&lt;&gt;()</code> の中で, <code>exit()</code>, 状態の差し替え, 次の状態の <code>entry()</code> 呼出しを行っている。<code>react()</code> 内の最後に書かないと混乱する。この作りは不味い。戻り値を <code>return</code> の引数にしろ、と強制されていればマダシモ、だった。


#### ☆ <a href="https://github.com/andrew-gresyk/HFSM2/">HFSM2</a>
MIT license. ゲームに役立つ機能があるらしい (未確認). 状態はクラスで, <code>FSM::State</code> から派生させる。状態機械は <code>FSM::Instance</code> インスタンス. 二つの書き方ができる。

 (1) イベントベースにしない。<code>FSM::Instance#update()</code> を呼び出すと状態クラスの <code>update(FullControl&amp; )</code> メソッドが呼び出される。そこで <code>FullControl#changeTo&lt;<var>stateClass</var>&gt;()</code> で直接遷移. 
    ちゃんと <code>update()</code> を抜けてから、次の状態の <code>enter()</code> が呼び出される。
    
 (2) イベントベース. <code>FSM::Instance#react(<var>event</var>)</code> を呼び出すと状態クラスの <code>react(<var>eventType&amp;</var>, FullControl&amp; )</code> メソッドが呼び出される。イベントの型でオーバーロードできる。


#### ▲ <a href="https://github.com/neilmendoza/ofxStateMachine/">ofxStateMachine</a>  
openFrameworks 向け. もはやメンテナンスされていない。▲direct transition <code>changeState(<var>stateStr</var>)</code>



そのほか:
 - UML-State-Machine-in-C もはやメンテナンスされていなさそう。
 - Another Finite State Machine (afsm) メンテナンスされていない。
 - 記事 <a href="https://www.codeproject.com/Articles/1087619/State-Machine-Design-in-Cplusplus-2">State Machine Design in C++ - CodeProject</a> をベースにしたもの。
 - https://github.com/Tencent/behaviac/  ゲームAI開発用.
 


### 重量級

イベントループも持ったもの。

 - <a href="https://github.com/robosoft-ai/SMACC2/">SMACC2 – State Machine Asynchronous C++</a> リアルタイム viewer がある。<i>Boost.Statechart</i> 上に実装。
   Tight integration with Robot Operating System (ROS) / SMACC2 for ROS 2

 - <a href="https://www.state-machine.com/products/qp">QP™ Real-Time Embedded Frameworks (RTEFs)</a> POSIX, Windows and macOS でも動くらしい。デザインについての議論スライド; <a href="https://www.cis.upenn.edu/~lee/06cse480/lec-HSM.pdf">Hierarchical State Machines - a Fundamentally Important Way of Design</a>
 
 - https://github.com/tklab-tud/uscxml/  SCXML interpreter and transformer/compiler. C++ライブラリ. Java, C#, Python, Luaバインディングあり。



## SCXML などの形式から C ソースコードを出力

考え方の筋が悪い。片方を修正したときに同期できない。これもいくつかある。
 - https://github.com/jp-embedded/scxmlcc/
 - <a href="https://smc.sourceforge.net/">SMC: The State Machine Compiler</a> 独自形式のファイルからC, C++, Java, ... を出力。

 - https://github.com/StateSmith/StateSmith/  yEd graph editor XML ファイルか PlantUML ファイルから, C/C++ ソースコードを出力する。
 
    
 
## 番外: 状態遷移図のテキストをヴィジュアル化

ソースコードだけではこんがらがりやすい。やたらと多い.

 * ☆ PlantUML  https://plantuml.com/ja/  一強。Javaで作られている. Graphviz に依存. コマンドラインオプションで, 画像だけでなく SCXML でも出力できる。
 * ☆ UMLet https://www.umlet.com/
 * JavaScript (クライアント側生成)
    - DotUML  https://dotuml.com/  using Graphviz Dot.
    - ☆ State Machine Cat  https://github.com/sverweij/state-machine-cat/  SCXML への出力にも対応
    - Mermaid   State diagrams もいけるが、簡易な図。


