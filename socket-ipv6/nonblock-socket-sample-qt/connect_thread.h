// -*- mode:c++ -*-

#ifndef CONNECT_THREAD_H
#define CONNECT_THREAD_H

#include <QThread>

// Qtにおけるマルチスレッドは、次の二つのやり方がある:
//  1. QThread から派生させる.
//     run() メソッドが呼び出される.
//  2. 標準の QThread と自前のワーカーオブジェクトを組み合わせる
//     myObject->moveToThread(targetThread)
//     これで myObject のスロットが <var>targetThread</var> スレッドで動く.
class ConnectThread: public QThread
{
    Q_OBJECT

public:
    ConnectThread(QObject* parent = nullptr);
    ~ConnectThread();

    void requestNewFortune(const std::string& hostName, int port);

protected:
    // start() を呼び出すと, 新しいスレッドで run() がコールバックされる.
    virtual void run() override;

signals:
    void fortuneGot(const QString& fortune);
    void error(int socketError, const QString& message);

private:
    std::string hostName;
    int port;
};

#endif
