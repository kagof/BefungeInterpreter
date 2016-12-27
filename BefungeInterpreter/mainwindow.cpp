/* * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                 *
 *      Kagof Befunge-93 Interpreter v1.0.0        *
 *        copyright (c) Karl Goffin 2016           *
 *    released under the open source MIT license   *
 *                                                 *
 *   http://github.com/Kagof/BefungeInterpreter    *
 *                                                 *
 *                                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * */


#include <QString>
#include <QStringList>
#include <QChar>
#include <QFileDialog>
#include <QDir>
#include <QBrush>
#include <QColor>
#include <QTextOption>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QTextFormat>
#include <QMessageBox>

#include <iostream>
#include <chrono>
#include <thread>
#include <stdexcept>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "file.h"
#include "codetorus.h"
#include "interpreter.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->resize(2000,1200);
    MainWindow::setWindowTitle(QString("Kagof Befunge Interpreter"));

    ui->actionSave_File->setEnabled(false);

    currentCharFormat = new QTextCharFormat();
    currentCharFormat->setBackground(QBrush(Qt::cyan));

    ui->sourceBox->setWordWrapMode(QTextOption::NoWrap);
    ui->inputBox->setReadOnly(true);
    ui->inputBox->setEnabled(false);
    ui->sourceBox->setFocus();

    defaultFormat = new QTextCharFormat(ui->sourceBox->currentCharFormat());

    mode = EDIT;
    savePrompt = false;
    fileIsOpen = false;

    keepPadding = true;
    keepRuntimeChanges = false;

    undo = false;
    redo = false;
    copy = false;
    slowTime = ui->speedBox->value();

    modified = false;
    running = false;
    started = false;
    submitted = false;
    resettable = false;


    // create and seed a random number generator for '?'
    // the random number generator is hosted in MainWindow because the Interpreter is deleted whenever we go to edit mode.
    rand_gen();
    rand_gen.seed(std::chrono::high_resolution_clock::now().time_since_epoch().count());

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

QString MainWindow::getSourceBoxText()
{
    return ui->sourceBox->toPlainText();
}

void MainWindow::setStackBoxText(QString s)
{
    ui->stackBox->setPlainText(s);
}

void MainWindow::output(char c)
{
    ui->outputBox->insertPlainText(QString(c));
}

void MainWindow::output(int i)
{
    ui->outputBox->insertPlainText(QString(std::to_string(i).c_str()));
}

void MainWindow::programFinished()
{
    if (started) ui->outputBox->setPlainText(QString(terp->getOutputStr()));
    running = false;
    started = false;
    ui->editRadioButton->setEnabled(true);
    ui->stepButton->setEnabled(false);

    ui->startButton->setEnabled(true);
    ui->startButton->setText("Reset");
    resettable = true;

    ui->debugButton->setEnabled(false);
    ui->slowButton->setEnabled(false);

    this->repaint();
    qApp->processEvents();
}

void MainWindow::invalidCharDialog(char c)
{
    QString msg("Invalid character '");
    msg.append(QChar(c));
    msg.append("' hit.");
    QMessageBox* box = new QMessageBox(QMessageBox::Warning, QString("Invalid"), msg, QMessageBox::Ok, this);
    box->exec();
}

bool MainWindow::isRunning()
{
    return running;
}

bool MainWindow::isStarted()
{
    return started;
}

char MainWindow::inputChar()
{
    ui->inputBox->setEnabled(true);
    ui->inputBox->setReadOnly(false);
    ui->inputBox->grabKeyboard();
    ui->inputBox->setFocus();
    ui->inputSubmitButton->setEnabled(true);
    ui->LFButton->setEnabled(true);

    //don't allow user to leave this mode
    ui->startButton->setEnabled(false);
    ui->debugButton->setEnabled(false);
    ui->stepButton->setEnabled(false);
    ui->slowButton->setEnabled(false);
    ui->editRadioButton->setEnabled(false);


    ui->inputLabel->setText(QString("Input Character"));
    while (true) {
        while (!submitted){  // loop until user clicks the submit button.
            this->repaint();
            qApp->processEvents();
        }

        QString tmpStr = ui->inputBox->text();

        //only accept strings of length 1
        if (tmpStr.length() == 1) {
            //reset the UI elements
            ui->inputLabel->setText(QString("Input"));
            ui->inputBox->clear();
            submitted = false;
            ui->inputBox->setEnabled(false);
            ui->inputBox->setReadOnly(true);
            ui->inputBox->releaseKeyboard();
            ui->inputSubmitButton->setEnabled(false);
            ui->LFButton->setEnabled(false);
            ui->startButton->setEnabled(true);
            ui->debugButton->setEnabled(true);
            ui->stepButton->setEnabled(true);
            ui->slowButton->setEnabled(true);
            ui->editRadioButton->setEnabled(true);

            return tmpStr.at(0).toLatin1();
        }

        //on a bad input, pop up a message box and try again
        ui->inputBox->clear();
        submitted = false;
        QMessageBox* box = new QMessageBox(QMessageBox::Warning, QString("Invalid"), QString("Please enter a valid character."), QMessageBox::Ok, this);
        box->exec();
    }
}

int MainWindow::inputInt()
{
    ui->inputBox->setEnabled(true);
    ui->inputBox->setReadOnly(false);
    ui->inputBox->grabKeyboard();
    ui->inputBox->setFocus();
    ui->inputSubmitButton->setEnabled(true);
    ui->startButton->setEnabled(false);

    //don't allow user to leave this mode
    ui->debugButton->setEnabled(false);
    ui->stepButton->setEnabled(false);
    ui->slowButton->setEnabled(false);
    ui->editRadioButton->setEnabled(false);


    ui->inputLabel->setText(QString("Input Integer"));
    while (true) {
        while (!submitted){  // loop until user clicks the submit button.
            this->repaint();
            qApp->processEvents();
        }

        //get input
        std::string tmpStr = ui->inputBox->text().toStdString();

        //if the input can be interpreted as an integer
        try {
            int i = std::stoi(tmpStr);

            //reset the UI elements
            ui->inputLabel->setText(QString("Input"));
            ui->inputBox->clear();
            submitted = false;
            ui->inputBox->setEnabled(false);
            ui->inputBox->setReadOnly(true);
            ui->inputBox->releaseKeyboard();
            ui->inputSubmitButton->setEnabled(false);
            ui->startButton->setEnabled(true);
            ui->debugButton->setEnabled(true);
            ui->stepButton->setEnabled(true);
            ui->slowButton->setEnabled(true);
            ui->editRadioButton->setEnabled(true);

            return i;
        }
        //on a bad input, pop up a message box and try again
        catch (std::invalid_argument e){
            ui->inputBox->clear();
            submitted = false;
            QMessageBox* box = new QMessageBox(QMessageBox::Warning, QString("Invalid"), QString("Please enter a valid integer."), QMessageBox::Ok, this);
            box->exec();
        }
    }
}

int MainWindow::randomBetweenOneAndThree()
{
    std::uniform_int_distribution<> range(0, 3);
    int val = range(rand_gen);
    return val;
}


void MainWindow::on_actionLoad_File_triggered()
{

    // make sure user is OK with closing current file
    if (modified || fileIsOpen) on_actionClose_File_triggered();

    QString filePath = QFileDialog::getOpenFileName(this,
                                                          "Select Befunge-93 source file to load",
                                                          QDir::currentPath(),
                                                          tr("Befunge file (*.bf);; Befunge-93 file (*.b93);; Text file (*.txt);;  All files (*.*)"));

    f = new File(this, filePath);
    fileIsOpen = true;
    ui->sourceBox->document()->setModified(false);
}

/*
 * turns on/off run mode.
 * */
void MainWindow::on_runRadioButton_toggled(bool checked)
{
    ui->editRadioButton->toggled(!checked);
    ui->sourceBox->setReadOnly(checked);  // user can't edit the text when in run mode.

    //Run Mode:
    if (checked) {

        if (!keepPadding) tmpOriginalProgram = ui->sourceBox->toPlainText();

        //if we already opened the file, and haven't changed anything, can use previously obtained values
        if (fileIsOpen && !this->windowTitle().endsWith(QString("*"))) torus = new CodeTorus(this, f->getWidth(), f->getHeight(), ui->sourceBox->toPlainText());

        else{
            QString st = ui->sourceBox->toPlainText();

            //ending with a newline character. the upcoming for loop does not work without it
            if (!st.endsWith("\n")) {
                st.append(QChar('\n'));
                this->setSourceBoxText(st);
            }

            //find the width and height of this text
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

            //remove the trailing newline, so that the split does not create a blank line at the end
            if (st.endsWith("\n")) {
                st.chop(1);
                this->setSourceBoxText(st);
            }

            QStringList sl = st.split(QChar('\n'));

            //pad each line with spaces, so that they are all the same length
            for (QStringList::iterator it = sl.begin(); it != sl.end(); it++) {
                while (it->length() < longest){
                    it->append(QChar(' '));
                }
            }

            st = sl.join(QChar('\n'));

            //re-append a trailing newline
            if (!st.endsWith("\n")) {
                st.append(QChar('\n'));
                this->setSourceBoxText(st);
            }
            this->setSourceBoxText(st);

            //create the torus
            torus = new CodeTorus(this, longest, numLines, st);            
        }

        if (keepPadding) tmpOriginalProgram = ui->sourceBox->toPlainText();

        //create the interpreter
        terp = new Interpreter(this, torus);

        //set the unsupported character mode (constructor defaults to ABORT)
        if (ui->actionIgnore->isChecked()) terp->setUnsupportedCharMode(Interpreter::IGNORE);
        if (ui->actionReflect->isChecked()) terp->setUnsupportedCharMode(Interpreter::REFLECT);

        //set the division & modulus by zero mode (constructor defaults to ASK for both)
        if (ui->actionPush_Zero->isChecked()) terp->setDivZeroMode(Interpreter::PUSHZERO);
        if (ui->actionPush_Zero_2->isChecked()) terp->setModZeroMode(Interpreter::PUSHZERO);
        if (ui->actionCrash->isChecked()) terp->setModZeroMode(Interpreter::CRASH);

        //highlight the first element
        cursor = new QTextCursor(ui->sourceBox->document());
        cursor->setPosition(1, QTextCursor::KeepAnchor);
        cursor->setCharFormat(*currentCharFormat);
        cursor->clearSelection();
        delete cursor;

        //we don't want the user to try to manually edit the source text when in run mode
        mode = RUN;
        ui->actionCut->setEnabled(false);
        ui->actionUndo->setEnabled(false);
        ui->actionRedo->setEnabled(false);
        ui->actionPaste->setEnabled(false);

    }
    else {
        //clean up
        mode = EDIT;
        ui->stackBox->clear();
        ui->inputBox->clear();
        ui->outputBox->clear();
        delete torus;
        delete terp;

        if (!keepRuntimeChanges) ui->sourceBox->setPlainText(tmpOriginalProgram);
        tmpOriginalProgram.clear();

        //grab keyboard
        ui->sourceBox->grabKeyboard();
        ui->sourceBox->setFocus();

        //get rid of the highlighted current PC
        cursor = new QTextCursor(ui->sourceBox->document());
        cursor->movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
        cursor->setCharFormat(*defaultFormat);
        cursor->clearSelection();
        delete cursor;
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

void MainWindow::on_menuFile_aboutToShow()
{
    bool editmd = (mode == EDIT);

    ui->actionLoad_File->setEnabled(editmd);
    ui->actionClose_File->setEnabled((editmd && (modified || fileIsOpen)));  // close button only enabled if we are in edit mode,
                                                                             // and we have modified the text or opened a file.
    if (fileIsOpen) ui->actionSave_File->setEnabled(editmd);
    else ui->actionSave_File->setEnabled(false);
    ui->actionSave_File_As->setEnabled(editmd);

}

void MainWindow::on_menuEdit_aboutToShow()
{
    ui->actionCopy->setEnabled(copy);
    ui->actionPaste->setEnabled(ui->sourceBox->canPaste());
    if (mode == EDIT){
        ui->actionCut->setEnabled(copy);
        ui->actionUndo->setEnabled(undo);
        ui->actionRedo->setEnabled(redo);
        ui->actionOverwrite_Mode->setEnabled(true);
    }
    else {
        ui->actionCut->setEnabled(false);
        ui->actionRedo->setEnabled(false);
        ui->actionUndo->setEnabled(false);
        ui->actionOverwrite_Mode->setEnabled(false);
    }
}

void MainWindow::on_menuOptions_aboutToShow()
{
    bool editmd = (mode == EDIT);
    ui->actionKeep_Padding_From_Run->setEnabled(editmd);
    ui->actionKeep_Runtime_Changes->setEnabled(editmd);
}

void MainWindow::on_sourceBox_modificationChanged(bool arg1)
{
    // if modified
    if (arg1) {
        // set title
        if (fileIsOpen) {
            std::string tmpstr = "Kagof Befunge Interpreter - " + f->getFilename() + " *";
            MainWindow::setWindowTitle(QString(tmpstr.c_str()));
        }
        else MainWindow::setWindowTitle(QString("Kagof Befunge Interpreter *"));

        // for enabling save
        modified = true;
    }
    // not modified
    else {
        // set title
        if (fileIsOpen) {
            std::string tmpstr = "Kagof Befunge Interpreter - " + f->getFilename();
            MainWindow::setWindowTitle(QString(tmpstr.c_str()));
        }
        else MainWindow::setWindowTitle(QString("Kagof Befunge Interpreter"));

        //for disabling save
        modified = false;
    }
}

void MainWindow::on_actionOverwrite_Mode_triggered(bool checked)
{
    ui->sourceBox->setOverwriteMode(checked);
}

void MainWindow::on_stepButton_clicked()
{
    terp->step();
    cursor = new QTextCursor(ui->sourceBox->document());
    cursor->movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    cursor->setCharFormat(*defaultFormat);
    cursor->clearSelection();
    delete cursor;
    cursor = new QTextCursor(ui->sourceBox->document());
    cursor->setPosition(torus->position());
    cursor->setPosition(torus->position()+1, QTextCursor::KeepAnchor);
    cursor->setCharFormat(*currentCharFormat);
    cursor->clearSelection();
    delete cursor;
}

void MainWindow::on_startButton_clicked()
{

    // if resettable is true, then the start button is functioning as the reset button.
    if (resettable) {
        // toggling the "run" radio button resets the program
        on_runRadioButton_toggled(false);
        on_runRadioButton_toggled(true);
        ui->startButton->setText("Start");
        resettable = false;
        return;
    }

    started = !started;
    while (started) {
        terp->step();
    }
    cursor = new QTextCursor(ui->sourceBox->document());
    cursor->movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
    cursor->setCharFormat(*defaultFormat);
    cursor->clearSelection();
    delete cursor;
    cursor = new QTextCursor(ui->sourceBox->document());
    cursor->setPosition(torus->position());
    cursor->setPosition(torus->position()+1, QTextCursor::KeepAnchor);
    cursor->setCharFormat(*currentCharFormat);
    cursor->clearSelection();
    delete cursor;
}

void MainWindow::on_speedBox_valueChanged(int arg1)
{
    slowTime = arg1;
}

void MainWindow::on_slowButton_clicked()
{
    running = !running;
    if (running) {
        ui->editRadioButton->setEnabled(false);
        ui->slowButton->setText(QString("Stop"));
        while (running) {
            on_stepButton_clicked();

            //update the UI
            this->repaint();
            qApp->processEvents();

            //wait for the desired amount of time
            std::this_thread::sleep_for(std::chrono::milliseconds(slowTime));
        }
        ui->slowButton->setText(QString("Slow"));
    }
    else {
        ui->editRadioButton->setEnabled(true);
        ui->slowButton->setText(QString("Slow"));
        this->repaint();
        qApp->processEvents();
    }
}

void MainWindow::on_inputSubmitButton_clicked()
{
    submitted = true;
}

void MainWindow::on_LFButton_clicked()
{
    ui->inputBox->setText(QString("\n"));
    submitted = true;
}

void MainWindow::on_actionIgnore_triggered(bool checked)
{
    if (checked){
        ui->actionReflect->setChecked(false);
        if (mode == RUN) terp->setUnsupportedCharMode(Interpreter::IGNORE);
    }
    else {
        if (ui->actionReflect->isChecked()){
            if (mode == RUN) terp->setUnsupportedCharMode(Interpreter::REFLECT);
        }
        else {
            if (mode == RUN) terp->setUnsupportedCharMode(Interpreter::ABORT);
        }
    }
}

void MainWindow::on_actionReflect_triggered(bool checked)
{
    if (checked) {
        ui->actionIgnore->setChecked(false);
        if (mode == RUN) terp->setUnsupportedCharMode(Interpreter::REFLECT);
    }
    else {
        if (ui->actionIgnore->isChecked()){
            if (mode == RUN) terp->setUnsupportedCharMode(Interpreter::IGNORE);
        }
        else {
            if (mode == RUN) terp->setUnsupportedCharMode(Interpreter::ABORT);
        }
    }
}

void MainWindow::on_actionSave_File_triggered()
{
    if (fileIsOpen) {
        QString sourceBoxText = ui->sourceBox->toPlainText();
        if (QFile(f->getDir()).exists()){
            QMessageBox::StandardButton reply;
            QString msg("File \"");
            msg.append(f->getDir());
            msg.append("\" already exists. Overwrite?");
            reply = (QMessageBox::StandardButton)QMessageBox::warning(this, "Confirm Overwrite", msg, QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);
            if (reply == QMessageBox::Cancel){
                return;
            }
            else if (reply == QMessageBox::No){
                on_actionSave_File_As_triggered();
            }
            else {
                if (f->saveFile(sourceBoxText)){
                    ui->sourceBox->document()->setModified(false);
                }
                else QMessageBox::warning(this, "Error", "File failed to save.", QMessageBox::Ok, QMessageBox::Ok);
            }
        }
        else {
            if (f->saveFile(sourceBoxText)){
                ui->sourceBox->document()->setModified(false);
            }
            else QMessageBox::warning(this, "Error", "File failed to save.", QMessageBox::Ok, QMessageBox::Ok);
        }
    }
}

void MainWindow::on_actionKeep_Padding_From_Run_triggered(bool checked)
{
    keepPadding = checked;
}

void MainWindow::on_actionKeep_Runtime_Changes_triggered(bool checked)
{
    keepRuntimeChanges = checked;
}

void MainWindow::on_actionSave_File_As_triggered()
{
    QString sourceBoxText = ui->sourceBox->toPlainText();
    QString filepath;
    if (!fileIsOpen) filepath = QFileDialog::getSaveFileName(this, "Save source as...",
                                                    QDir::currentPath(),
                                                    tr("Befunge file (*.bf);; Befunge-93 file (*.b93);; Text file (*.txt);;  All files (*.*)"));

    else filepath = QFileDialog::getSaveFileName(this, "Save source as...",
                                                 QString(f->getFilepath().c_str()),
                                                 tr("Befunge file (*.bf);; Befunge-93 file (*.b93);; Text file (*.txt);;  All files (*.*)"));

    if (f->saveFileAs(filepath, sourceBoxText)){
        if (!fileIsOpen) {
            f = new File(this, filepath);
            fileIsOpen = true;
            ui->sourceBox->document()->setModified(false);
        }
        else {
            f->setDir(filepath);
            f->loadFile();
            ui->sourceBox->document()->setModified(false);
        }
    }
    else QMessageBox::warning(this, "Error", "File failed to save.", QMessageBox::Ok, QMessageBox::Ok);
}

void MainWindow::on_actionClose_File_triggered()
{

    // if the file is open
    if (fileIsOpen){

        // confirm close
        QString msg("Close file \"");
        msg.append(f->getDir());
        msg.append("\"?");
        QMessageBox::StandardButton reply;
        reply = (QMessageBox::StandardButton)QMessageBox::warning(this, "Close File", msg, QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Yes);
        if (reply == QMessageBox::Yes){

            //if the file was modified, ask if the user wants to save changes before exiting.
            if (modified){
                QMessageBox::StandardButton reply2;
                reply2 = (QMessageBox::StandardButton)QMessageBox::warning(this, "Save File", "Save changes before closing?", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);
                if (reply2 == QMessageBox::Yes){
                    on_actionSave_File_triggered();  // save
                }
                else if (reply2 == QMessageBox::Cancel) return;
            }
            // close the file
            delete f;
            fileIsOpen = false;
            ui->sourceBox->setPlainText(QString());
            ui->sourceBox->document()->setModified(false);
            on_sourceBox_modificationChanged(false);

        }
        else {
            return;  // user aborted close
        }
    }
    // file is not open
    else {

        // confirm close
        QMessageBox::StandardButton reply;
        reply = (QMessageBox::StandardButton)QMessageBox::warning(this, "Close File", "Clear source?", QMessageBox::Yes | QMessageBox::Cancel, QMessageBox::Yes);
        if (reply == QMessageBox::Yes){

            //if the file was modified, ask if the user wants to save changes before exiting.
            if (modified){
                QMessageBox::StandardButton reply2;
                reply2 = (QMessageBox::StandardButton)QMessageBox::warning(this, "Save File", "Save changes before closing?", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel, QMessageBox::Yes);
                if (reply2 == QMessageBox::Yes){
                    on_actionSave_File_As_triggered();  // save
                }
                else if (reply2 == QMessageBox::Cancel) return;
            }
            // clear the source
            if (fileIsOpen){
                delete f;
                fileIsOpen = false;
            }
            ui->sourceBox->setPlainText(QString());
            ui->sourceBox->document()->setModified(false);
            on_sourceBox_modificationChanged(false);
        }
        else return;
    }
}

void MainWindow::on_actionAsk_For_Input_triggered(bool checked)
{
    if (checked) {
        ui->actionPush_Zero->setChecked(false);
        if (mode == RUN) terp->setDivZeroMode(Interpreter::ASK);
    }
    else {
        ui->actionAsk_For_Input->setChecked(true);  // this is the default option, so the only way to uncheck is by selecting the other option.
        on_actionAsk_For_Input_triggered(true);
    }
}

void MainWindow::on_actionPush_Zero_triggered(bool checked)
{
    if (checked) {
        ui->actionAsk_For_Input->setChecked(false);
        if (mode == RUN) terp->setDivZeroMode(Interpreter::PUSHZERO);
    }
    else {
        ui->actionAsk_For_Input->setChecked(true);
        on_actionAsk_For_Input_triggered(true);
    }
}

void MainWindow::on_actionAsk_For_Input_2_triggered(bool checked)
{
    if (checked) {
        ui->actionPush_Zero_2->setChecked(false);
        ui->actionCrash->setChecked(false);
        if (mode == RUN) terp->setModZeroMode(Interpreter::ASK);
    }
    else {
        ui->actionAsk_For_Input_2->setChecked(true);  // this is the default option, so the only way to uncheck is by selecting another option.
        on_actionAsk_For_Input_2_triggered(true);
    }
}

void MainWindow::on_actionPush_Zero_2_triggered(bool checked)
{
    if (checked) {
        ui->actionAsk_For_Input_2->setChecked(false);
        ui->actionCrash->setChecked(false);
        if (mode == RUN) terp->setModZeroMode(Interpreter::PUSHZERO);
    }
    else {
        ui->actionAsk_For_Input_2->setChecked(true);
        on_actionAsk_For_Input_2_triggered(true);
    }
}

void MainWindow::on_actionCrash_triggered(bool checked)
{
    if (checked) {
        ui->actionAsk_For_Input_2->setChecked(false);
        ui->actionPush_Zero_2->setChecked(false);
        if (mode == RUN) terp->setModZeroMode(Interpreter::CRASH);
    }
    else {
        ui->actionAsk_For_Input_2->setChecked(true);
        on_actionAsk_For_Input_2_triggered(true);
    }
}
