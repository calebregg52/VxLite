#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QFile>

#include "test.h"

QT_BEGIN_NAMESPACE
namespace Ui { class VxLite; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void on_SelectFileButton_clicked();
    void on_SaveFileButton_clicked();

    QString link = "https://github.com/calebregg52/VxLite";
    QString version = "VxLite v1.0";

    QString filenameOpen;
    QString filenameSave;

    //Test *test = new Test;


signals:
    void onBenchmark();

public slots:
    void onPrint(QString);
    void onClear();
    void on_actionVersion_triggered();
    void on_actionSource_triggered();
    void on_actionExit_triggered();
    void on_actionOpen_triggered();
    void on_actionBenchmark_triggered();
    void on_actionSaveAs();
    void onCompress();
    void onDecompress();
    void clearFiles();



private:
    Ui::VxLite *ui;
};
#endif // MAINWINDOW_H
