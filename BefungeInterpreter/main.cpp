#include <QApplication>
#include <QString>
#include <iostream>
#include <string>

#include "mainwindow.h"
#include "interpreter.h"
#include "codetorus.h"

int main(int argc, char *argv[])
{
    bool testmode = false;
    //testmode = true;

    QApplication a(argc, argv);
    MainWindow w;

    if (testmode){
        CodeTorus* t;
        Interpreter i(&w, t);
        i.push('a');
        i.push('f');
        i.push('\n');
        i.push('n');

        QString s = i.stackToQString();
        std::cout << s.toStdString().c_str() << std::endl;

        return 0;
    }


    w.show();

    return a.exec();
}
