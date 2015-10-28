#include "findrunnerwidget.h"
#include "ui_findrunnerwidget.h"
#include "runsplugin.h"

#include <qf/qmlwidgets/framework/mainwindow.h>

#include <qf/core/model/sqltablemodel.h>
#include <qf/core/sql/querybuilder.h>

namespace qfm = qf::core::model;
namespace qfs = qf::core::sql;

namespace Runs {

static RunsPlugin *runsPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	auto *plugin = qobject_cast<Runs::RunsPlugin *>(fwk->plugin("Runs"));
	QF_ASSERT_EX(plugin != nullptr, "Bad plugin");
	return plugin;
}

FindRunnerWidget::FindRunnerWidget(int stage_id, QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::FindRunnerWidget)
	//, m_stageId(stage_id)
{
	ui->setupUi(this);
	ui->edFindRunner->setFocus();
	ui->edFindRunner->setTable(runsPlugin()->runsTable(stage_id));
	connect(ui->edFindRunner, &FindRunnerEdit::runnerSelected, this, &FindRunnerWidget::runnerSelected);
}

FindRunnerWidget::~FindRunnerWidget()
{
	delete ui;
}

void FindRunnerWidget::focusLineEdit()
{
	ui->edFindRunner->setFocus();
}
/*
void FindRunnerWidget::reload()
{
	qfLogFuncFrame();
	if(!isVisible())
		return;
}
*/
}
