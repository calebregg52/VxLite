#ifndef TEST_H
#define TEST_H

#include <QString>
#include <QWidget>

class Test
{
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
