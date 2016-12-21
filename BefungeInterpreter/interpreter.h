#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <QString>
#include <stack>

#include "mainwindow.h"
#include "codetorus.h"



class Interpreter
{
public:
    Interpreter(MainWindow *parent, CodeTorus *torus);
    void step();
    QString stackToQString();
    void push(char c);
    char pop();

private:
    bool inputMode;
    MainWindow* parent;
    CodeTorus* torus;
    std::stack<char>* st;

signals:
    void programTerminates();

};

#endif // INTERPRETER_H
