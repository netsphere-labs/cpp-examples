# UML StateMachines

UML 1.5 では statechart diagram という名前だった. UML2 で StateMachines パッケージに名前変更。


## qt-trafficlight-handwritten

状態遷移のサンプル。Qt5/Qt6 両対応. これをベースにした; <a href="https://doc.qt.io/qt-6/qtstatemachine-statemachine-trafficlight-example.html">Traffic Light Example | Qt State Machine 6.4.1</a>

状態 (state) が X のときにイベント E が発生すると, 次の状態 Y に遷移する、と定義していく。

このサンプルでは, 黄色信号は赤に行くこともあれば青に行くこともある。遷移させるイベントはいずれも timeout. イベントが同じなのに行き先が二つ以上の場合は、状態を二つに分けてやる。

Qt の状態遷移は signal & slot の仕組みを活用しており、タイムアウトや他のイベント (ボタンを押したなど) も統一的に扱える。他方、Qt イベントループへの登録が前提になっており、かなり重い処理になっている。

状態遷移図を手書きで実装すべきではない。穴が開いているかどうかまるで分からない。Qt は scxml をインポートしてそのまま実行できるので、GUI で状態遷移図を描くとよい。




## boost-ext-sml

Based on https://github.com/ubeh/fsm_examples/






## 軽量な有限状態機械 (Finite State Machine; FSM) ライブラリ

`QStateMachine` は, 1クラスのなかで状態を管理したいといったような, 軽い用途には使いにくい。軽量ライブラリを探した。山のようにあるが、まぁまぁ人気のものを挙げる。

<table>
  <tr><td>☆ <a href="https://boost-ext.github.io/sml/">[Boost::ext].SML</a>
    <td>軽量なもののなかでメジャー。<i>Boost.Statechart</i> はパフォーマンスが悪く, Boost Meta State Machine (MSM) はコンパイルの時間・メモリを大量に食う。
しかし, C++ の超絶技巧を駆使しており、エラーが出た場合など、挙動がさっぱり分からない。

  <tr><td>○ <a href="https://github.com/erikzenker/hsm/">Hana State Machine (HSM)</a> 
    <td>[Boost::ext].SML と Boost.MSM の実装の仕方を Boost.Hana で再実装し, 独自のメタプログラミングコードの大きさを縮小。パフォーマンス傾向は MSM に似ている。なかなかいい。


  <tr><td>○ <a href="https://github.com/digint/tinyfsm/">TinyFSM</a>
    <td>イベントがクラス.
▲direct transition <code>transit&lt;<var>stateClass</var>&gt;()</code>
<code>send_event(<var>event</var>)</code> で遷移させることもできる。
      小さくてよさそう。
      
  <tr><td>▲ <a href="https://github.com/andrew-gresyk/HFSM2/">HFSM2</a>
    <td>▲direct transition  <code>changeTo&lt;<var>stateClass</var>&gt;()</code>

  <tr><td>▲ <a href="https://github.com/neilmendoza/ofxStateMachine/">ofxStateMachine</a>  
    <td>▲direct transition <code>changeState(<var>stateStr</var>)</code>
</table>



## 状態遷移図のテキストをヴィジュアル化

ソースコードだけではこんがらがりやすい。多い.

 * ☆ PlantUML  https://plantuml.com/ja/  Javaベース. Graphviz に依存. SCXML に変換できる。
 * ☆ UMLet https://www.umlet.com/
 * JavaScript (クライアント側生成)
    - DotUML  https://dotuml.com/  using Graphviz Dot.
    - ☆ State Machine Cat  https://github.com/sverweij/state-machine-cat/  SCXML にも対応



## SCXML

交換用のフォーマット。<a href="https://alexzhornyak.github.io/SCXML-tutorial/">SCXML-tutorial | SCXML illustrated examples and tutorials</a>

実装 https://github.com/tklab-tud/uscxml/  C++ライブラリ. Java, C#, Python, Luaバインディングあり。



## SCXML から C ソースコードを出力

考え方の筋が悪い。片方を修正したときに同期できない。

これもいくつかある。https://github.com/jp-embedded/scxmlcc/


    
 