#include "runstabledialogwidget.h"
#include "ui_runstabledialogwidget.h"

#include <Event/eventplugin.h>

#include <qf/qmlwidgets/framework/mainwindow.h>

#include <qf/core/utils.h>
#include <qf/core/assert.h>

using qf::qmlwidgets::framework::getPlugin;
using Event::EventPlugin;

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

void RunsTableDialogWidget::reload(int stage_id, int class_id, bool show_offrace, const QString &sort_column, int select_competitor_id)
{
	QString class_name = getPlugin<EventPlugin>()->classNameById(class_id);
	setTitle(tr("Stage %1 Class %2").arg(stage_id).arg(class_name));
	runsTableWidget()->reload(stage_id, class_id, show_offrace, sort_column, select_competitor_id);
}
