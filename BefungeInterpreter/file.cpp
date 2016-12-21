#include <fstream>
#include <string>
#include <ios>
#include <cstddef>

#include "file.h"


File::File(MainWindow* parent, QString dir)
{
    this->parent = parent;
    this->dir = dir;
    this->loadFile();
}

bool File::loadFile() {
    width = -1;
    height = 0;
    std::ifstream in(dir.toStdString());
    if (!in.is_open()){
        return false;  // return false if the file failed to open.
    }

    //get the height & width of the input
    std::string currentLine;
    int numLines = 0;
    while (in.is_open() && std::getline(in, currentLine)){
        if ((int)currentLine.length() > width){
            width = (int)currentLine.length();
        }
        numLines ++;
    }
    if (numLines > height) {
        height = numLines;
    }


    // return to the beginning of the file
    in.clear();
    in.seekg(0, std::ios::beg);

    //input into the UI
    while (in.is_open() && std::getline(in, currentLine)){
        int i;
        for (i = 0; i < currentLine.length(); i++){
            parent->addToSourceBox(currentLine.at(i));
        }
        while (i < width){
            parent->addToSourceBox(' ');
            i++;
        }
        parent->addToSourceBox('\n');
    }

    std::string tmp = dir.toStdString();

    std::size_t lastSlash = tmp.find_last_of("/\\");
    filename = tmp.substr(lastSlash+1);

    return true;
}

int File::getWidth()
{
    return width;
}

int File::getHeight()
{
    return height;
}

std::string File::getFilename()
{
    return filename;
}
