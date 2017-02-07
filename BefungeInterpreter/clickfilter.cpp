#include "clickfilter.h"
#include "mainwindow.h"

#include <QMouseEvent>
#include <QPoint>
#include <QTextCursor>
#include <QDebug>

ClickFilter::ClickFilter(MainWindow *parentMainWindow, QObject *parent) : QObject(parent)
{
    this->parentMainWindow = parentMainWindow;
}

bool ClickFilter::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonRelease && parentMainWindow->isInRunMode()){
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        QPoint pos = mouseEvent->pos();
        QPlainTextEdit *sourceBox = static_cast<QPlainTextEdit *>(watched->parent());
        QTextCursor curs = sourceBox->cursorForPosition(pos);

        if (curs.position() >= sourceBox->toPlainText().length()) return false;

        bool breakpoint = parentMainWindow->toggleBreakpoint(curs.position());
        parentMainWindow->highlightBreakpoint(curs, breakpoint);

        return true;
    }
    else return false;
}
