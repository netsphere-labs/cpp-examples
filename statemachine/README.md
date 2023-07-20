# UML StateMachines

UML 1.5 では statechart diagram という名前だった. UML2 で StateMachines パッケージに名前変更。




## サンプルプログラム

### `qt-trafficlight-handwritten/`

状態遷移のサンプル。Qt5/Qt6 両対応. これをベースにした; <a href="https://doc.qt.io/qt-6/qtstatemachine-statemachine-trafficlight-example.html">Traffic Light Example | Qt State Machine 6.4.1</a>

状態 (state) X のときにイベント E が発生すると, 次の状態 Y に遷移する、と定義していく。状態は `QState` クラスインスタンスとする。`addTransition()` メソッドでイベントと遷移先を登録する。

このサンプルでは, 黄色信号は赤に行くこともあれば青に行くこともある。遷移させるイベントはいずれも timeout. イベントが同じなのに行き先が二つ以上の場合は、状態を二つに分けてやる。

Qt の状態遷移は signal &amp; slot の仕組みを活用しており、タイムアウトや他のイベント (ボタンを押したなど) も統一的に扱える。他方、Qt イベントループへの登録が前提になっており、かなり重い処理になっている。

状態遷移図を手書きで実装すべきではない。穴が開いているかどうかまるで分からない。Qt は SCXML をインポートしてそのまま実行できるので、GUI で状態遷移図を描くとよい。



### `qt-trafficlight-scxml-dynamic/`

Qt5/Qt6 両対応。SCXML ファイルをそのまま利用する信号機サンプル。Based on <a href="https://doc.qt.io/qt-6/qtscxml-trafficlight-widgets-dynamic-example.html">SCXML Traffic Light (Dynamic, Widgets)</a>


SCXML は状態遷移図の交換用フォーマット。<a href="https://alexzhornyak.github.io/SCXML-tutorial/">SCXML-tutorial | SCXML illustrated examples and tutorials</a>. Qt Creator は GUI で編集できる。

このサンプルは, SCXML をコンパイルせずに、そのまま読み込む。`QScxmlStateMachine` クラスインスタンスが SCXML の内容を保持する。`connectToState()` メソッドでコールバック関数を登録する。コールバック関数は, 次の宣言のように, `bool` 型の引数を持つこと。

```c++
  void mySlot(bool active);
```

SCXML は <code>&lt;send event="startGoingGreen" delay="3s" /&gt;</code> のようにタイムアウトをこの中で書けるため、インタプリタは必然的にイベントループを必要とする。





### `boost-ext-sml/`

Based on https://github.com/ubeh/fsm_examples/

<i>[Boost::ext].SML</i> は軽量な状態機械ライブラリでメジャー。名前が紛らわしいが, <i>Boost</i> の一部ではなく, Boost には依存しない。

SML では state はクラスではなく, アクションの関数。ある状態のときにイベントが発生すると、アクションがコールバックされる。




### `sml-visualize/`

SML の状態遷移のコード定義を PlantUML のテキストとして出力する。これそのまま; https://github.com/boost-ext/sml/blob/master/example/plant_uml.cpp

コンパイルには C++17 が必要 (C++14 には `if constexpr` がない). Template metaprogramming (TMP) の権化みたいなコードになっている。正直, 可変長テンプレート引数は理解の範囲を超える.





## 有限状態機械 (Finite State Machine; FSM) ライブラリ

Qt の `QStateMachine` は, 1クラスのなかで状態を管理したいといったような, 軽い用途には使いにくい。状態機械ライブラリを探した。山のようにあるが、まぁまぁ人気のものを挙げる。

UML2 では, <i>behavior (behavioural) state machines</i> and <i>protocol state machines</i> という区分を挙げている。求められる機能性の違いは微妙だが、前者は状態が動名詞になりやすい。

State が内部状態や振る舞いを持つため, クラスとして定義できるのが前者、状態がクラスではなくアクション関数を定義するのが後者。。




### 軽量ライブラリ

#### ☆ <a href="https://boost-ext.github.io/sml/">[Boost::ext].SML</a>
<i>Boost.Statechart</i> はパフォーマンスが悪く, Boost Meta State Machine (MSM) はコンパイルの時間・メモリを大量に食う。SML は, パフォーマンスは MSM 並で、コンパイルが多少は軽くなっている。
しかし, C++ の超絶技巧を駆使しており、エラーが出た場合など挙動がさっぱり分からないのが傷。


#### ▲ <a href="https://github.com/erikzenker/hsm/">Hana State Machine (HSM)</a> 
[Boost::ext].SML と Boost.MSM の実装の仕方を <i>Boost.Hana</i> で再実装し, 独自実装のメタプログラミングコードの大きさを縮小。パフォーマンス傾向は MSM に似ている。悪くない。しかし, 黒魔術であることには変わらないので, SML を使えばよい。



#### ▲ <a href="https://github.com/digint/tinyfsm/">TinyFSM</a>
大きさが小さい。リソース制約がキツい用途では, よさそう。

イベントはクラス <code>tinyfsm::Event</code> から派生させる。状態もクラス. 継承関係がちょっと変わっている。<code>tinyfsm::Fsm&lt;&gt;</code> クラスを派生させて状態機械クラスを定義し、そこから各状態クラスを派生させる。

<code>状態機械クラス#dispatch(<var>event</var>)</code> を呼び出すと状態クラスの <code>react(<var>eventType&amp;</var> )</code> メソッドが呼び出される。そのなかで <code>transit&lt;<var>stateClass</var>&gt;()</code> で遷移させる。
複数の状態機械を設ける場合は <code>tinyfsm::FsmList&lt;StateClass, ...&gt;</code> を使う。<code>template dispatch&lt;eventType&gt;(event)</code> で全状態機械にイベントを配信できる。

▲<code>transit&lt;&gt;()</code> の中で, <code>exit()</code>, 状態の差し替え, 次の状態の <code>entry()</code> 呼出しを行っている。<code>react()</code> 内の最後に書かないと混乱する。この作りは不味い。戻り値を <code>return</code> の引数にしろ、と強制されていればマダシモ、だった。



#### ☆ <a href="https://github.com/andrew-gresyk/HFSM2/">HFSM2</a>
MIT license. ゲームに役立つ機能があるらしい (未確認). 状態はクラスで, <code>FSM::State</code> から派生させる。状態機械は <code>FSM::Instance</code> インスタンス. 二つの書き方ができる。

 (1) イベントベースにしない方法。<code>FSM::Instance#update()</code> を呼び出すと, 現在の状態クラスの <code>update(FullControl&amp; )</code> メソッドが呼び出される。そこで <code>FullControl#changeTo&lt;<var>stateClass</var>&gt;()</code> で直接遷移. 
    ちゃんと <code>update()</code> を抜けてから、次の状態の <code>enter()</code> が呼び出される。
    
 (2) イベントベース. <code>FSM::Instance#react(<var>event</var>)</code> を呼び出すと, 現在の状態クラスの <code>react(<var>eventType&amp;</var>, FullControl&amp; )</code> メソッドが呼び出される。イベントの型でオーバーロードできる。




#### ▲ <a href="https://github.com/neilmendoza/ofxStateMachine/">ofxStateMachine</a>  
openFrameworks 向け. リリース実績なし。もはやメンテナンスされていない。▲direct transition <code>changeState(<var>stateStr</var>)</code>

状態はクラス, `itg::ofxState<>` から派生させる。





そのほか:

 - ▲ <a href="https://github.com/kiishor/UML-State-Machine-in-C/">kiishor/UML-State-Machine-in-C: A minimalist UML State machine framework for finite state machine and hierarchical state machine in C</a>
   もはやメンテナンスされていなさそう。
   解説記事 https://www.codeproject.com/Articles/5273346/State-Machine-Framework-in-C
   Plain C 用!  handler 関数で定義する。State は内部状態を持たない (クラスではない)。


 - <a href="https://github.com/zmij/afsm/">zmij/afsm: C++14 Finite State Machine library</a>
   Another Finite State Machine (afsm) メンテナンスされていない。
   Boost MSM にインスパイアされた。遷移表を作るタイプ (状態はクラスではない).
```c++
using initial_state = idle;
using transitions = transition_table <
    /*  State       Event                   Next        Action  Guard           */
    tr< idle,       events::load_goods,     loading,    none,   check_amount    >,
    tr< loading,    events::load_done,      idle,       none,   none            >
>;
```


 - <a href="https://github.com/Tencent/behaviac/">Tencent/behaviac: behaviac is a framework of the game AI development, and it also can be used as a rapid game prototype design tool. behaviac supports the behavior tree, finite state machine and hierarchical task network(BT, FSM, HTN)</a>
   ゲームAI開発用.


 - <a href="https://github.com/endurodave/StateMachine/">endurodave/StateMachine: A compact C++ finite state machine (FSM) implementation that&#39;s easy to use on embedded and PC-based systems.</a>
   記事 <a href="https://www.codeproject.com/Articles/1087619/State-Machine-Design-in-Cplusplus-2">State Machine Design in C++ - CodeProject</a> をベースにしたもの。
   状態機械をクラス `StateMachine` から派生させて作る。状態はクラスではない。まず、次のように各状態を宣言する。
```c++
    // Define the state machine state functions with event data type
    STATE_DECLARE(Motor, 	Idle,			NoEventData)
    STATE_DECLARE(Motor, 	Stop,			NoEventData)
    STATE_DECLARE(Motor, 	Start,			MotorData)
    STATE_DECLARE(Motor, 	ChangeSpeed,	MotorData)
```
   すると, 状態機械クラスに次のようなメソッド `ST_*` が生える
```c++
void ST_Start(const MotorNMData* data)
```


記事  https://faramira.com/reusable-finite-state-machine-using-cpp/

記事  https://www.aleksandrhovhannisyan.com/blog/implementing-a-finite-state-machine-in-cpp/





### 重量級

イベントループも持ったもの。組み込み向けになる。

#### <a href="https://github.com/robosoft-ai/SMACC2/">SMACC2 – State Machine Asynchronous C++</a>
リアルタイム viewer がある。<i>Boost.Statechart</i> 上に実装。
Tight integration with Robot Operating System (ROS) / SMACC2 for ROS 2


#### <a href="https://www.state-machine.com/products/qp">QP™ Real-Time Embedded Frameworks (RTEFs)</a>
POSIX, Windows and macOS でも動くらしい。デザインについての議論スライド; <a href="https://www.cis.upenn.edu/~lee/06cse480/lec-HSM.pdf">Hierarchical State Machines - a Fundamentally Important Way of Design</a>
`QP::QHsm` クラスと, そこから派生した `QP::QMsm` クラスが中心。

状態は `QStateHandler` 関数で表す (クラスではない)。`Q_STATE_DECL()` マクロで宣言し, `Q_STATE_DEF()` マクロで関数を定義する。状態機械クラスのメソッドになる。


#### ▲ <a href="https://github.com/tklab-tud/uscxml/">tklab-tud/uscxml: SCXML interpreter and transformer/compiler written in C/C++ with bindings to Java, C#, Python and Lua</a>
最後のリリースが 2017 年2月. 廃れたか.

C++で作られている。ベンチマークあり, fastest (対 scxmlcc (限界が低い), apache, qt, lxsc (Lua用)). 






## SCXML などの形式から C ソースコードを出力

考え方の筋が悪い。片方を修正したときに同期できない。SCXML インタプリタのほうがよい。これもいくつかある。

 - ▲ <a href="https://github.com/jp-embedded/scxmlcc/">jp-embedded/scxmlcc: The SCXML state machine to C++ compiler</a>
   <i>uscxml</i> を使え.

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


