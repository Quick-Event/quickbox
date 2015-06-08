#include "mainwindow.h"

#include <qf/qmlwidgets/framework/centralwidget.h>

#include <QLabel>

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags) :
	Super(parent, flags)
{
}

MainWindow::~MainWindow()
{
}

void MainWindow::whenPluginsLoaded()
{
	centralWidget()->setActivePart("Competitors", true);
}
