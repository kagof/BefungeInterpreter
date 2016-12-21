#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <QString>
#include <stack>
#include <random>

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
    QString getOutputStr();

private:
    bool stringmode;
    MainWindow* parent;
    CodeTorus* torus;
    std::stack<char>* st;
    QString outputStr;
    void output(char c);
    void output(int i);
    std::mt19937 rand_gen;

};

#endif // INTERPRETER_H
