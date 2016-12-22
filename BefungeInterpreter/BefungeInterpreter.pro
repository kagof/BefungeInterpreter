#-------------------------------------------------
#
# Project created by QtCreator 2016-12-19T17:16:51
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 5): QT += widgets

TARGET = BefungeInterpreter
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    file.cpp \
    codetorus.cpp \
    interpreter.cpp

HEADERS  += mainwindow.h \
    file.h \
    codetorus.h \
    interpreter.h

FORMS    += mainwindow.ui

RESOURCES += \
    images.qrc
	
RC_FILE = bficon.rc
