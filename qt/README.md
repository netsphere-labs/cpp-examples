# Qt サンプル

## trafficlight-handwritten

状態遷移のサンプル。Qt5/Qt6 両対応.

これをベースにした; <a href="https://doc.qt.io/qt-6/qtstatemachine-statemachine-trafficlight-example.html">Traffic Light Example | Qt State Machine 6.4.1</a>

状態 (state) が X のときにイベント E が発生すると, 次の状態 Y に遷移する、と定義していく。

黄色信号のときにイベント timeout が発生したとき、次が赤に行くか青に行くか。イベントが同じなのに行き先が二つ以上の場合は、状態を二つに分けてやる。

