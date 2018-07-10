#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "application.h"
#include "appclioptions.h"

#include <qf/core/log.h>

#include <QContextMenuEvent>
#include <QMenu>
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
	AppCliOptions *cliopts = app->cliOptions();

	setContextMenuPolicy(Qt::ContextMenuPolicy::ActionsContextMenu);
	{
		QAction *a = new QAction(tr("Toggle full screen"), this);
		a->setShortcut(QKeySequence::FullScreen);
		connect(a, &QAction::triggered, [this]() {
			if(isFullScreen()) {
				showNormal();
			}
			else {
				showFullScreen();
			}
		});
		addAction(a);
	}
	{
		QAction *a = new QAction(tr("Increase font size"), this);
		a->setShortcut(QKeySequence::ZoomIn);
		connect(a, &QAction::triggered, [this]() {
			Application *app = Application::instance();
			AppCliOptions *cliopts = app->cliOptions();
			int fs = cliopts->fontScale();
			fs += 10;
			cliopts->setFontScale(fs);
			ui->frmContent->resetCellSize();
		});
		addAction(a);
	}
	{
		QAction *a = new QAction(tr("Decrease font size"), this);
		a->setShortcut(QKeySequence::ZoomOut);
		connect(a, &QAction::triggered, [this]() {
			Application *app = Application::instance();
			AppCliOptions *cliopts = app->cliOptions();
			int fs = cliopts->fontScale();
			fs -= 10;
			cliopts->setFontScale(fs);
			ui->frmContent->resetCellSize();
		});
		addAction(a);
	}

	QVariantMap event_info = app->eventInfo();
	//qfInfo() << event_info;
	ui->lblHeadCenter->setText(event_info.value("name").toString());
	ui->lblHeadRight->setText(event_info.value("date").toString());
	int stage_cnt = event_info.value("stageCount").toInt();
	int curr_stage = cliopts->stage();
	if(!cliopts->stage_isset()) {
		curr_stage = event_info.value("currentStageId").toInt();
		if(curr_stage == 0)
			curr_stage = 1;
		qfInfo() << "Setting stage id to current stage:" << curr_stage;
		cliopts->setStage(curr_stage);
	}
	if(stage_cnt > 1)
		ui->lblHeadLeft->setText(tr("E%1").arg(curr_stage));
	else
		ui->lblHeadLeft->setText(QString());

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
