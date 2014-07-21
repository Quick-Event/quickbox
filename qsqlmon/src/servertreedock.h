#ifndef SERVERTREEDOCK_H
#define SERVERTREEDOCK_H

#include <QDockWidget>

#include "ui_servertreewidget.h"

class QAction;
class QActionGroup;
class QMenu;

class ServerTreeDock : public QDockWidget
{
    Q_OBJECT
public:
    ServerTreeDock(QWidget *parent = 0, Qt::WindowFlags flags = 0);

    //QMenu *menu;

protected:
    //virtual void contextMenuEvent(QContextMenuEvent *event);
    //virtual void polishEvent(QEvent *);
public:
    Ui::ServerTreeWidget ui;
    /*
private:
    void allow(Qt::DockWidgetArea area, bool allow);
    void place(Qt::DockWidgetArea area, bool place);

private slots:
    void changeClosable(bool on);
    void changeMovable(bool on);
    void changeFloatable(bool on);
    void changeFloating(bool on);

    void allowLeft(bool a);
    void allowRight(bool a);
    void allowTop(bool a);
    void allowBottom(bool a);

    void placeLeft(bool p);
    void placeRight(bool p);
    void placeTop(bool p);
    void placeBottom(bool p);
    */
};

#endif
