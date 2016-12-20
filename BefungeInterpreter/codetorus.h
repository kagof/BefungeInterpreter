#ifndef CODETORUS_H
#define CODETORUS_H

#include <QString>

#include "mainwindow.h"

enum direction{UP, DOWN, LEFT, RIGHT};

class CodeTorus
{
public:
    CodeTorus(MainWindow *parent, int width, int height, QString chars);

    char getChar(int row, int col);
    void putChar(char c, int row, int col);
    void setChars(QString chars);
    void changeDirection(direction newDrctn);
    int getX();
    int getY();
    char getCurrentChar();
    void next();

private:
    MainWindow *parent;
    QString chars;
    int width, height;
    direction drctn;
    int x;
    int y;
    char currentChar;

};

#endif // CODETORUS_H
