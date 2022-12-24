#include "mainwindow.h"
#include <QApplication>
#include <QIODevice>
#include <QTextStream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // SCXMLファイルから状態機械を作る
    QScxmlStateMachine *machine = QScxmlStateMachine::fromFile(
                ":/statemachine.scxml");
    if (!machine->parseErrors().isEmpty()) {
        QTextStream errs(stderr, QIODevice::WriteOnly);
        const auto errors = machine->parseErrors();
        for (const QScxmlError &error : errors)
            errs << error.toString();

        return -1;
    }

    MyMainWindow w(machine);
    w.show();
    machine->setParent(&w);
    machine->start();

    return a.exec();
}
