#ifndef TEST_H
#define TEST_H

#include <QString>
#include <QWidget>

class Test : QWidget
{
    Q_OBJECT
public:

    Test();
    ~Test();

    //void PrintMainWindow(QString);
    //void VxLiteTest();

signals:
    void onPrint(QString message);
    void onClear();

public slots:
    void VxLiteTest();


};

#endif // TEST_H
