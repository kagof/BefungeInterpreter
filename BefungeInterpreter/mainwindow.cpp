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

    mode = EDIT;
    fileIsOpen = false;
    undo = false;
    redo = false;
    copy = false;

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addToSourceBox(char c)
{
    ui->sourceBox->insertPlainText(QString(c));
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
    fileIsOpen = true;
}

void MainWindow::on_runRadioButton_toggled(bool checked)
{
    ui->editRadioButton->toggled(!checked);
    ui->sourceBox->setReadOnly(checked);
    if (checked) {
        if (fileIsOpen) torus = new CodeTorus(this, f->getWidth(), f->getHeight(), ui->sourceBox->toPlainText());
        else{
            QString st = ui->sourceBox->toPlainText();
            int longest = -1;
            int current = 0;
            int numLines = 0;
            for (int i = 0; i < st.length(); i++){
                if (st.at(i).toLatin1()=='\n'){
                    if (current > longest) longest = current;
                    current = 0;
                    numLines++;
                }
                else current ++;
            }
            torus = new CodeTorus(this, longest, numLines, st);
        }
        mode = RUN;
        ui->actionCut->setEnabled(false);
        ui->actionUndo->setEnabled(false);
        ui->actionRedo->setEnabled(false);
        ui->actionPaste->setEnabled(false);
    }
    else {
        mode = EDIT;
        ui->stackBox->clear();
        ui->inputBox->clear();
        ui->outputBox->clear();
    }
    ui->sourceBox->setUndoRedoEnabled(checked);

    ui->startButton->setEnabled(checked);
    ui->debugButton->setEnabled(checked);
    ui->stepButton->setEnabled(checked);
    ui->slowButton->setEnabled(checked);
    ui->speedBox->setEnabled(checked);
    ui->speedLabel->setEnabled(checked);

    ui->inputLabel->setEnabled(checked);
    ui->stackLabel->setEnabled(checked);
    ui->outputLabel->setEnabled(checked);
}

void MainWindow::on_sourceBox_textChanged()
{
    ui->runRadioButton->setEnabled(((int)ui->sourceBox->toPlainText().length() != 0));
}

void MainWindow::on_actionCopy_triggered()
{
    ui->sourceBox->copy();
}

void MainWindow::on_actionCut_triggered()
{
    ui->sourceBox->cut();
}

void MainWindow::on_actionPaste_triggered()
{
    if (mode == EDIT) ui->sourceBox->paste();
    else ui->inputBox->paste();
}

void MainWindow::on_actionUndo_triggered()
{
    ui->sourceBox->undo();
}

void MainWindow::on_actionRedo_triggered()
{
    ui->sourceBox->redo();
}

void MainWindow::on_sourceBox_redoAvailable(bool b)
{
    redo = b;
}

void MainWindow::on_sourceBox_copyAvailable(bool b)
{
    copy = b;
}

void MainWindow::on_sourceBox_undoAvailable(bool b)
{
    undo = b;
}

void MainWindow::on_menuEdit_aboutToShow()
{
    ui->actionCopy->setEnabled(copy);
    ui->actionPaste->setEnabled(ui->sourceBox->canPaste());
    if (mode == EDIT){
        ui->actionCut->setEnabled(copy);
        ui->actionUndo->setEnabled(undo);
        ui->actionRedo->setEnabled(redo);
    }
    else {
        ui->actionCut->setEnabled(false);
        ui->actionRedo->setEnabled(false);
        ui->actionUndo->setEnabled(false);
    }
}
