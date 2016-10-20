#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "application.h"

#include <qf/core/log.h>

#include <QSettings>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	ui->frmTitle->setStyleSheet("QFrame {background: gold}\n"
								"QLabel {font-size: 20pt}");
	ui->lblHeadCenter->setStyleSheet("font-weight: bold");

	Application *app = Application::instance();
	QVariantMap event_info = app->eventInfo();
	//qfInfo() << event_info;
	ui->lblHeadCenter->setText(event_info.value("name").toString());
	ui->lblHeadRight->setText(event_info.value("date").toString());
	int stage_cnt = event_info.value("stageCount").toInt();
	int curr_stage = event_info.value("currentStageId").toInt();
	if(stage_cnt > 1)
		ui->lblHeadLeft->setText(tr("E%1").arg(curr_stage));

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
