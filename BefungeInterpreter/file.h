#ifndef FILE_H
#define FILE_H
#include <QString>
#include <vector>
#include <string>

#include "mainwindow.h"

class File
{
public:
    static const int DEFAULT_WIDTH = 80;
    static const int DEFAULT_HEIGHT = 25;
    File(MainWindow* parent, QString dir);
    bool saveFile();
    bool saveFileAs(QString newDir);
    bool loadFile();
    int getWidth();
    int getHeight();
    std::string getFilename();

private:
    MainWindow *parent;
    QString dir;
    int width, height;
    std::string filename;
};

#endif // FILE_H
