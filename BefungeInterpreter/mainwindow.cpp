#include <QString>
#include <QFileDialog>
#include <QDir>
#include <iostream>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "file.h"
#include "codetorus.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->resize(2000,1200);
    MainWindow::setWindowTitle(QString("Kagof Befunge Interpreter"));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addToSourceBox(char c)
{
    ui->sourceBox->insertPlainText(QString(c));
    std::cout << ui->sourceBox->toPlainText().toStdString() << std::endl;
}

void MainWindow::setSourceBoxText(QString s)
{
    ui->sourceBox->setPlainText(s);
}

void MainWindow::on_actionLoad_File_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(this,
                                                          "Select Befunge-93 source file to load",
                                                          QDir::currentPath(),
                                                          tr("Befunge file (*.bf);; Text file (*.txt);;  All files (*.*)"));
    f = new File(this, filePath);
}

void MainWindow::on_runRadioButton_toggled(bool checked)
{
    ui->editRadioButton->toggled(false);
    ui->sourceBox->setReadOnly(true);
    torus = new CodeTorus(this, f->getWidth(), f->getHeight(), ui->sourceBox->toPlainText());
}
