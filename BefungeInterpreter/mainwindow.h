#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class File;
class CodeTorus;

enum Mode{EDIT, RUN};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void addToSourceBox(char c);
    void setSourceBoxText(QString s);

private slots:
    void on_actionLoad_File_triggered();

    void on_runRadioButton_toggled(bool checked);

    void on_sourceBox_textChanged();

    void on_actionCopy_triggered();

    void on_actionCut_triggered();

    void on_actionPaste_triggered();

    void on_actionUndo_triggered();

    void on_actionRedo_triggered();

    void on_sourceBox_redoAvailable(bool b);

    void on_sourceBox_copyAvailable(bool b);

    void on_sourceBox_undoAvailable(bool b);

    void on_menuEdit_aboutToShow();

private:
    Ui::MainWindow *ui;

    File* f;
    CodeTorus* torus;
    bool fileIsOpen;
    Mode mode;
    bool undo, redo, copy;
};

#endif // MAINWINDOW_H
