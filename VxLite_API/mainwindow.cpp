#include "mainwindow.h"
#include "ui_mainwindow.h"

//#include "test.h"
#include <VxLite.hpp>

#include <QFile>
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


    connect(ui->clearButton, &QPushButton::released, this, &MainWindow::clearFiles);



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


void MainWindow::clearFiles()
{
    filenameSave.clear();
    ui->OpenFileText->clear();
    filenameOpen.clear();
    ui->SaveAsText->clear();
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

    QString temp = "";

    // Path to vls file on-disk.
    std::string path = filenameOpen.toStdString();

    std::cout << "test1" << std::endl;

    // Create a vls_file from API call
    VxLite::vls_file* file = VxLite::OpenFromFile(path);

    std::cout << "test2" << std::endl;


    // Check that the file is open
    if(file == nullptr)
    {
        std::cout<<"Fatal: Could not open file at path"<<std::endl;
        return ;
    }

    const size_t raw_size = file->xs*file->ys*file->zs*file->bpv;

    temp.append("Loaded ");
    temp.append(path);
    ui->MainTextEdit->append(temp);

    temp.clear();
    temp.append("Dimensions are ");
    temp.append(QString::number(file->xs));
    temp.append("x");
    temp.append(QString::number(file->ys));
    temp.append("x");
    temp.append(QString::number(file->zs));

    ui->MainTextEdit->append(temp);

    temp.clear();
    temp.append(QString::number(file->bpv));
    temp.append(" bytes per voxel");

    ui->MainTextEdit->append(temp);

    // Now, create a bytespace and a compression context for the data
    VxLite::sbs space;
    VxLite::ctx context(&space);

    // Always unfilter after compressing, and filter again before compressing!!
    ui->MainTextEdit->append("Unfiltering bytespace...");
    context.UnfilterSpace();

    // Fix / optimize filters
    //ui->MainTextEdit->append("Attempting filter optimization...");
    //context.OptimizeFilters();

    // Refilter for compression and writing
    ui->MainTextEdit->append("Re-filtering bytespace...");
    context.FilterSpace();

    ui->MainTextEdit->append("Compressing...");
    context.Compress(*file);

    ui->MainTextEdit->append("Writing to disk...");
    VxLite::SaveToFile(*file, "fixed.vls");

    // Cleanup
    delete file;
    ui->MainTextEdit->append("Done!");

}

// Decompress selected file
void MainWindow::onDecompress()
{

    // Path to vls file on-disk.
    std::string path = filenameOpen.toStdString();

    // Create a vls_file from API call
    VxLite::vls_file* file = VxLite::OpenFromFile(path);

    // Check that the file is open
    if(file == nullptr)
    {
        std::cout<<"Fatal: Could not open file at path"<<std::endl;
        return ;
    }


    QString temp;

    const size_t raw_size = file->xs*file->ys*file->zs*file->bpv;

    // Now, create a bytespace and a compression context for the data
    VxLite::sbs space;
    VxLite::ctx context(&space);

    // Decompression API call
    temp.clear();
    temp.append("Decompressing LZ4 data...");
    ui->MainTextEdit->append(temp);

    //std::cout<<"Decompressing LZ4 data..."<<std::endl;
    context.Decompress(*file);
    temp.clear();
    temp.append("Decompressed ");
    temp.append(QString::number(raw_size/(1024.f*1024.f)));
    temp.append("MB");
    ui->MainTextEdit->append(temp);
    temp.clear();

    // Always unfilter after compressing, and filter again before compressing!!
    std::cout<<"Unfiltering bytespace..."<<std::endl;
    context.UnfilterSpace();

    //VxLite::SaveToFile(*file, filenameSave.toStdString());

    ui->MainTextEdit->append("Done!");

    delete file;
}
