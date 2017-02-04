#include "clickfilter.h"
#include "mainwindow.h"

#include <QMouseEvent>
#include <QPoint>
#include <QTextCursor>
#include <QDebug>

ClickFilter::ClickFilter(MainWindow *parentMainWindow, QPlainTextEdit *sourceBox, QObject *parent) : QObject(parent)
{
    this->parentMainWindow = parentMainWindow;
    this->sourceBox = sourceBox;
}

bool ClickFilter::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonRelease && parentMainWindow->isInRunMode()){
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        QPoint pos = mouseEvent->pos();
        QTextCursor curs = sourceBox->cursorForPosition(pos);

        if (curs.position() >= sourceBox->toPlainText().length()) return false;

        bool breakpoint = parentMainWindow->toggleBreakpoint(curs.position());
        parentMainWindow->highlightBreakpoint(curs, breakpoint);

        return true;
    }
    else return false;
}
