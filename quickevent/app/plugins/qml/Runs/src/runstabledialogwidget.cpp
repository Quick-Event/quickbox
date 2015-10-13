#include "runstabledialogwidget.h"
#include "ui_runstabledialogwidget.h"

#include <Event/eventplugin.h>

#include <qf/qmlwidgets/framework/mainwindow.h>

#include <qf/core/utils.h>

static Event::EventPlugin* eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Event::EventPlugin*>(fwk->plugin("Event"));
	QF_ASSERT_EX(plugin != nullptr, "Bad event plugin!");
	return plugin;
}

RunsTableDialogWidget::RunsTableDialogWidget(QWidget *parent) :
	Super(parent),
	ui(new Ui::RunsTableDialogWidget)
{
	ui->setupUi(this);

	setPersistentSettingsId("RunsTableDialogWidget");
}

RunsTableDialogWidget::~RunsTableDialogWidget()
{
	delete ui;
}

RunsTableWidget *RunsTableDialogWidget::runsTableWidget()
{
	return ui->runsTableWidget;
}

void RunsTableDialogWidget::reload(int stage_id, int class_id, const QString &sort_column, int select_competitor_id)
{
	QString class_name = eventPlugin()->classNameById(class_id);
	setTitle(tr("Stage %1 Class %2").arg(stage_id).arg(class_name));
	runsTableWidget()->reload(stage_id, class_id, sort_column, select_competitor_id);
}
