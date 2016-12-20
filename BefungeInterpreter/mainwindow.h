#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class File;
class CodeTorus;

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

private:
    Ui::MainWindow *ui;

    File* f;
    CodeTorus* torus;
};

#endif // MAINWINDOW_H
