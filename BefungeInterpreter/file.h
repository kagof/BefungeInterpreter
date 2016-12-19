#ifndef FILE_H
#define FILE_H
#include <QMainWindow>
#include <QString>

class File
{
public:
    static const int DEFAULT_WIDTH = 80;
    static const int DEFAULT_HEIGHT = 25;
    File(QMainWindow parent, QString dir);
    bool SaveFile();
    bool loadFile();

private:
    QMainWindow parent;
    QString dir;
    char charAr[][];
};

#endif // FILE_H
