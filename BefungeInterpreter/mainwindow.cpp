#include <QString>
#include <QFileDialog>
#include <QDir>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->resize(2000,1200);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionLoad_File_triggered()
{
    QString filePaths = QFileDialog::getOpenFileName(this,
                                                          "Select Befunge-93 source file to load",
                                                          QDir::currentPath(),
                                                          tr("Befunge file (*.bf);; Text file (*.txt);;  All files (*.*)"));
}
