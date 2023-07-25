#include "mainwindow.h"
#include "ui_mainwindow.h"

//#include "test.h"
#include <VxLite.hpp>

#include<QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QDesktopServices>

#include <iostream>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::VxLite)
{
    ui->setupUi(this);

    connect(ui->SelectFileButton, &QPushButton::released, this, &MainWindow::on_SelectFileButton_clicked);

    connect(ui->SaveAsButton, &QPushButton::released, this, &MainWindow::on_SaveFileButton_clicked);

    connect(ui->CompressButton, &QPushButton::released, this, &MainWindow::onCompress);
    connect(ui->DecompressButton, &QPushButton::released, this, &MainWindow::onDecompress);



    //connect(test, &Test::onPrint, this, &MainWindow::onPrint);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_SelectFileButton_clicked()
{
    QString fileContent;

    QString filenameOpen= QFileDialog::getOpenFileName(this, "Choose File");


    if(filenameOpen.isEmpty())
        return;

    QFile file(filenameOpen);

    if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
        return;

    // Save file pointer
    MainWindow::filenameOpen = filenameOpen;

    // Set TextEdit to file directory
    ui->OpenFileText->clear();
    ui->OpenFileText->append(filenameOpen);

    // Print original size
    ui->MainTextEdit->append(filenameOpen + "\t" + QString::number(file.size()) + " Bytes\n");

    file.close();

    std::cout << MainWindow::filenameOpen.toStdString() << std::endl;
}

void MainWindow::on_SaveFileButton_clicked()
{
    QString fileContent;

    QString filenameSave= QFileDialog::getSaveFileName(this, "Choose File");


    if(filenameSave.isEmpty())
        return;

    QFile file(filenameSave);

    if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
        return;

    // Save file pointer
    MainWindow::filenameSave = filenameSave;

    // Set TextEdit to file directory
    ui->SaveAsText->clear();
    ui->SaveAsText->append(filenameSave);

    // Print original size
    ui->MainTextEdit->append(filenameSave + "\t" + QString::number(file.size()) + " Bytes\n");

    file.close();


    std::cout << MainWindow::filenameSave.toStdString() << std::endl;
}


// Print to main window
void MainWindow::onPrint(QString text)
{
    ui->MainTextEdit->insertPlainText(text);
}

// Clear main window
void MainWindow::onClear()
{
    ui->MainTextEdit->clear();
}


// Display version number
void MainWindow::on_actionVersion_triggered()
{
    QMessageBox::information(this,"Version", version);
}

// Open GitHub Page in default link
void MainWindow::on_actionSource_triggered()
{
    QDesktopServices::openUrl(QUrl(link));
}

// Exit VxLite Application
void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

// Open a File in VxLite
void MainWindow::on_actionOpen_triggered()
{
    on_SelectFileButton_clicked();
}


void MainWindow::on_actionBenchmark_triggered()
{
    //test->VxLiteTest();
}

void MainWindow::on_actionSaveAs()
{
    on_SaveFileButton_clicked();
}



// Compress selected file
void MainWindow::onCompress()
{
    // Check if file is valid
    if(filenameOpen.isEmpty())
    {
        ui->MainTextEdit->append("No file name specified.");
        return;
    }

    QFile file(filenameOpen);

    if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
        return;

    //VxLite::vls_file* outfile = new VxLite::vls_file({0, 0, 0, 0, 0, 0, nullptr, nullptr})

    VxLite::vls_file *vls = VxLite::OpenFromFile(filenameOpen.toStdString());

    ui->MainTextEdit->append("Compressing...");

    VxLite::SaveToFile(*vls, filenameSave.toStdString());


    file.close();

}

// Decompress selected file
void MainWindow::onDecompress()
{
    // Check if file is valid
    if(filenameOpen.isEmpty())
    {
        ui->MainTextEdit->append("No file name specified.");
        return;
    }

    QFile file(filenameSave);

    if (!file.open(QIODevice::ReadWrite | QIODevice::Text))
        return;


    VxLite::vls_file *vls = VxLite::OpenFromFile(filenameOpen.toStdString());

    ui->MainTextEdit->append("Decompressing...");

    VxLite::SaveToFile(*vls, filenameSave.toStdString());

    file.close();
}





