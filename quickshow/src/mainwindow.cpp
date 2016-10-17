#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->frmTitle->setStyleSheet("QFrame {background: yellow}\n"
								"QLabel {font-size: 20pt}");
	ui->lblHeadCenter->setStyleSheet("font-weight: bold");
}

MainWindow::~MainWindow()
{
	delete ui;
}
