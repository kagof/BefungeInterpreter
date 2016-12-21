#include "codetorus.h"

CodeTorus::CodeTorus(MainWindow *parent, int width, int height, QString chars)
{
    this->parent = parent;
    this->width = width;
    this->height = height;
    this->chars = chars;
    this->drctn = RIGHT;
    this->x = 0;
    this->y = 0;
    this->currentChar = getChar(x, y);
}

char CodeTorus::getChar(int row, int col)
{
    if (row < height && col < width){
        return chars.at(col + width*row).toLatin1();
    }
    else return (char) 0;
}

void CodeTorus::putChar(char c, int row, int col)
{
    if (row < height && col < width){
        chars.replace(col + (width+1)*row, 1, new QChar(c),1);
        parent->setSourceBoxText(chars);
    }

/* TODO:
 * adjust size of torus if user tries to insert element outside torus range.
 * */
/*
    if (row >= height){
        height = row + 1;
        QString appender;
        for (int i = 0; i < width; i++)
    }
*/
}

void CodeTorus::setChars(QString chars)
{
    this->chars = chars;
}

void CodeTorus::changeDirection(direction newDrctn)
{
    this->drctn = newDrctn;
}

int CodeTorus::getX()
{
    return x;
}

int CodeTorus::getY()
{
    return y;
}

char CodeTorus::getCurrentChar()
{
    return currentChar;
}

void CodeTorus::next()
{
    switch(drctn) {
        case(RIGHT):
        {
            x = (x + 1) % width;
        }
        case(LEFT):
        {
            x = (x - 1) % height;
        }
        case(UP):
        {
            y = (y - 1) % height;
        }
        case(DOWN):
        {
            y = (y + 1) % height;
        }
    }
    currentChar = getChar(x, y);
}

QString CodeTorus::getChars()
{
    return chars;
}
