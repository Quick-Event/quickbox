#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSettings>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->frmTitle->setStyleSheet("QFrame {background: yellow}\n"
								"QLabel {font-size: 20pt}");
	ui->lblHeadCenter->setStyleSheet("font-weight: bold");
	{
		QSettings settings;
		restoreGeometry(settings.value("mainWindow/geometry").toByteArray());
		restoreState(settings.value("mainWindow/state").toByteArray());
	}
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	QSettings settings;
	settings.setValue("mainWindow/geometry", saveGeometry());
	settings.setValue("mainWindow/state", saveState());
	QMainWindow::closeEvent(event);
}
