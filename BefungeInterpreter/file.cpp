#include <iostream>

#include "file.h"


File::File(QMainWindow parent, QString dir)
{
    this->parent = parent;
    this->dir = dir;
    this->loadFile();
}

bool File::loadFile() {

}
