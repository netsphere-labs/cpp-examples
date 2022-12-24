
#include <QStateMachine>
#include <QFinalState>
#include <QTimer>
#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "trafficlightwidget.h"

//! [2]
QState *createLightState(LightWidget *light, int duration, QState *parent = nullptr)
{
    assert(light);

    QState *lightState = new QState(parent);
    QTimer *timer = new QTimer(lightState);
    timer->setInterval(duration);
    timer->setSingleShot(true);

    // このサンプルがヤヤコシイのは、状態 lightState のなかに入れ子の状態 timing を作るから.
    QState *timing = new QState(lightState);

    // 状態 timing は、初期状態でタイマを起動し, timeout signal で終端になるだけ.
    // 状態 timing に入った (entered) ら, 一つのライトのスロット turnOn を呼び出す.
    QObject::connect(timing, &QAbstractState::entered, light, &LightWidget::turnOn);
    QObject::connect(timing, &QAbstractState::entered, timer, QOverload<>::of(&QTimer::start));
    QObject::connect(timing, &QAbstractState::exited, light, &LightWidget::turnOff);
    QFinalState *done = new QFinalState(lightState);
    // タイムアウトしたら終端にする.
    //   => 外側の状態 lightState のほうで, これをトリガにして状態遷移させる.
    timing->addTransition(timer,            // sender
                          &QTimer::timeout, // signal
                          done);            // QAbstractState *target
    lightState->setInitialState(timing);

    return lightState;
}
//! [2]


MyMainWindow::MyMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // widget に紐付けるのでここで設定する. しかし今ひとつの気もする。
    QStateMachine *machine = new QStateMachine(this); // QObject parent

    TrafficLightWidget* lightWidget = ui->lightWidget;
    // 状態を作る
    QState *redGoingYellow = createLightState(lightWidget->redLight(), 3000);
    redGoingYellow->setObjectName("redGoingYellow");
    QState *yellowGoingGreen = createLightState(lightWidget->yellowLight(), 1000);
    yellowGoingGreen->setObjectName("yellowGoingGreen");
    redGoingYellow->addTransition(redGoingYellow, &QState::finished, yellowGoingGreen);
    QState *greenGoingYellow = createLightState(lightWidget->greenLight(), 3000);
    greenGoingYellow->setObjectName("greenGoingYellow");
    yellowGoingGreen->addTransition(yellowGoingGreen, &QState::finished, greenGoingYellow);
    QState *yellowGoingRed = createLightState(lightWidget->yellowLight(), 1000);
    yellowGoingRed->setObjectName("yellowGoingRed");
    greenGoingYellow->addTransition(greenGoingYellow, &QState::finished, yellowGoingRed);
    yellowGoingRed->addTransition(yellowGoingRed, &QState::finished, redGoingYellow);

    machine->addState(redGoingYellow);
    machine->addState(yellowGoingGreen);
    machine->addState(greenGoingYellow);
    machine->addState(yellowGoingRed);
    machine->setInitialState(redGoingYellow);

    machine->start();
}

MyMainWindow::~MyMainWindow()
{
    delete ui;
}

