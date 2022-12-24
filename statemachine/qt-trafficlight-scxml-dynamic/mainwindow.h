#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtScxml/QScxmlStateMachine>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MyMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MyMainWindow(QScxmlStateMachine* machine, QWidget *parent = nullptr);
    ~MyMainWindow();

private slots:
    void toggleWorking(bool pause);

private:
    Ui::MainWindow *ui;
    QScxmlStateMachine* m_machine;
};
#endif // MAINWINDOW_H
