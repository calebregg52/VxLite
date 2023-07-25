#include "mainwindow.h"

#include <QApplication>
#include <test.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w = new MainWindow();
    //Test *test = new Test;

    //QObject::connect(test, &Test::onClear, w, &MainWindow::onClear);


    w.show();
    return a.exec();
}



