
// Qt Socket Sample

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <stdio.h>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), m_thread(nullptr)
{
    ui->setupUi(this);

    // Qt6 .ui 上で connect すると, ビルドできない.  -> connect はソースコードで行え. ヒドい.
    // メニューに接続
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::onFileExit);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onHelpAbout);

    // 次のプロトタイプ宣言にすると, Auto-Connect 黒魔法が掛かる.
    // void on_<object name>_<signal name>(<signal parameters>);
    // See https://doc.qt.io/qt-5/designer-using-a-ui-file.html#widgets-and-dialogs-with-auto-connect
    // コメントアウト:
    //connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::on_pushButton_clicked);

    connect(ui->hostLineEdit, &QLineEdit::textChanged, this, &MainWindow::enableGetFortuneButton);
    connect(ui->portLineEdit, &QLineEdit::textChanged, this, &MainWindow::enableGetFortuneButton);
}

void MainWindow::delete_thread()
{
    if (m_thread) {
        // 終了を促す. 強制終了 QThread::terminate() は使うべきではない.
        m_thread->quit();
        // 子スレッドが run() から抜けるまで待つ.
        m_thread->wait();
        delete m_thread; // デストラクタは終了を待たない.
        m_thread = nullptr;
    }
}

MainWindow::~MainWindow()
{
    delete_thread();
    delete ui;
}

// Slot: ファイル -> 終了
void MainWindow::onFileExit()
{
    QCoreApplication::quit();
}

// Slot
void MainWindow::onHelpAbout()
{
    QMessageBox::about(this, tr("Qt Socket Sample"),
                tr("This <i>Qt Socket Sample</i> demonstrates how to use "
                          "a combination of socket and thread."));
}

// Slot
void MainWindow::on_pushButton_clicked()
{
    if (m_thread)
        return;

    int port = ui->portLineEdit->text().toInt();
    if (port <= 0 || port > 65535) {
      QMessageBox msgBox(this);
      msgBox.setText("ポート番号は 1..65535");
      msgBox.setStandardButtons(QMessageBox::Ok);
      msgBox.setIcon(QMessageBox::Critical);
      msgBox.exec();
      return;
    }

    ui->pushButton->setEnabled(false);
    // 値がないことによるボタン無効と区別する
    QApplication::setOverrideCursor(Qt::WaitCursor);  

    m_thread = new ConnectThread();
    connect(m_thread, &ConnectThread::fortuneGot, this, &MainWindow::showFortune);
    connect(m_thread, &ConnectThread::error, this, &MainWindow::displayError);

    m_thread->requestNewFortune( ui->hostLineEdit->text().toStdString(),
                                 port );
}

// Slot: 子スレッドからコールバック
void MainWindow::showFortune(const QString& fortune)
{
    ui->textBrowser->setText(fortune);

    delete_thread();
    QApplication::restoreOverrideCursor();
    enableGetFortuneButton();
}

// Slot
void MainWindow::displayError(int socketError, const QString& message)
{
    ui->textBrowser->setText(message);

    delete_thread();
    QApplication::restoreOverrideCursor();
    enableGetFortuneButton();
}


// Slot
void MainWindow::enableGetFortuneButton()
{
    bool enable;
    if (m_thread)
        enable = false;
    else {
        enable = !ui->hostLineEdit->text().isEmpty() &&
                 !ui->portLineEdit->text().isEmpty();
    }
    ui->pushButton->setEnabled(enable);
}

