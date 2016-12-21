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

char CodeTorus::getChar(int x, int y)
{
    if (y < height && x < width){
        return chars.at(x + (width + 1)*y).toLatin1();
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

int CodeTorus::position()
{
    return x + (width + 1)*y;
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
            break;
        }
        case(LEFT):
        {
            if (x == 0) x = width - 1;
            else x = x - 1;
            break;
        }
        case(UP):
        {
            if (y == 0) y = height - 1;
            else y = y - 1;
            break;
        }
        case(DOWN):
        {
            y = (y + 1) % height;
            break;
        }
    }
    currentChar = getChar(x, y);
}

QString CodeTorus::getChars()
{
    return chars;
}
