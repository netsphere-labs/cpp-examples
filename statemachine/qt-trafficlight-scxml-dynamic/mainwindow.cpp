#include "mainwindow.h"
#include "./ui_mainwindow.h"

MyMainWindow::MyMainWindow(QScxmlStateMachine* machine, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), m_machine(machine)
{
    ui->setupUi(this);

    // 状態機械のコールバック
    TrafficLightWidget* widget = ui->trafficLight;
    machine->connectToState("red",
                            widget->redLight(), &LightWidget::switchLight);
    // #yellow と #redGoingGreen が入れ子になっている。なので、黄色も付く.
    machine->connectToState("redGoingGreen",
                            widget->redLight(), &LightWidget::switchLight);
    machine->connectToState("yellow",
                            widget->yellowLight(), &LightWidget::switchLight);
    // 故障
    machine->connectToState("blinking",
                            widget->yellowLight(), &LightWidget::switchLight);
    machine->connectToState("green",
                            widget->greenLight(), &LightWidget::switchLight);

    connect(widget->runButton(), &QAbstractButton::toggled,
            this, &MyMainWindow::toggleWorking);
}

MyMainWindow::~MyMainWindow()
{
    delete ui;
}

void MyMainWindow::toggleWorking(bool pause)
{
    m_machine->submitEvent(pause ? "smash" : "repair");
}
