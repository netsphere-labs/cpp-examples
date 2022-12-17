#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "connect_thread.h"
#include <QMainWindow>
#include <QLineEdit>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

// 拡張された構文
public slots:
    void onFileExit();
    void onHelpAbout();

    void on_pushButton_clicked();
    void enableGetFortuneButton();

protected slots:
    // 子スレッドからのコールバック
    void showFortune(const QString& fortune);
    void displayError(int socketError, const QString& message);

private:
    Ui::MainWindow* ui;
    // 簡単にするため, 1回ごとに作る.
    ConnectThread* m_thread;

    void delete_thread();
};
#endif // MAINWINDOW_H
