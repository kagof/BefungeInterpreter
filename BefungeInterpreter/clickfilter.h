#ifndef CLICKFILTER_H
#define CLICKFILTER_H

#include <QObject>
#include <QEvent>
#include <QPlainTextEdit>
class MainWindow;

/*
 * this class intercepts click events on the QPlainTextEdit sourceBox
 * */
class ClickFilter : public QObject
{
    Q_OBJECT
public:
    ClickFilter(MainWindow* parentMainWindow, QObject *parent = 0);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

signals:

public slots:

private:
    MainWindow *parentMainWindow;
};

#endif // CLICKFILTER_H
