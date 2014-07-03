#include "mainwindow.h"

#include <QLabel>

MainWindow::MainWindow(QWidget *parent, Qt::WindowFlags flags) :
	Super(parent, flags)
{
	setCentralWidget(new QLabel("Central Widget"));
}

MainWindow::~MainWindow()
{
}
