#include "runsplugin.h"
#include "thispartwidget.h"
#include "../runswidget.h"
#include "drawing/drawingganttwidget.h"

#include <Event/eventplugin.h>

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/framework/dockwidget.h>
#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/menubar.h>

#include <qf/core/log.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/qmlwidgets/dialogs/dialog.h>
#include <qf/qmlwidgets/dialogs/messagebox.h>

#include <QQmlEngine>

namespace qfw = qf::qmlwidgets;
namespace qff = qf::qmlwidgets::framework;
//namespace qfd = qf::qmlwidgets::dialogs;
namespace qfs = qf::core::sql;

using namespace Runs;

static Event::EventPlugin* eventPlugin()
{
	qf::qmlwidgets::framework::MainWindow *fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	qf::qmlwidgets::framework::Plugin *plugin = fwk->plugin("Event");
	return qobject_cast<Event::EventPlugin*>(plugin);
}

RunsPlugin::RunsPlugin(QObject *parent)
	: Super(parent)
{
	connect(this, &RunsPlugin::installed, this, &RunsPlugin::onInstalled, Qt::QueuedConnection);
}

RunsPlugin::~RunsPlugin()
{
}

void RunsPlugin::onInstalled()
{
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	m_partWidget = new ThisPartWidget();
	fwk->addPartWidget(m_partWidget, manifest()->featureId());

	connect(fwk->plugin("Event"), SIGNAL(editStartListRequest(int,int,int)), this, SLOT(onEditStartListRequest(int,int,int)), Qt::QueuedConnection);

	emit nativeInstalled();

	auto *a_draw = m_partWidget->menuBar()->actionForPath("drawing");
	a_draw->setText("&Drawing");
	{
		qfw::Action *a = new qfw::Action("Classes layout");
		a_draw->addActionInto(a);
		connect(a, &qfw::Action::triggered, [this]()
		{
			auto *w = new drawing::DrawingGanttWidget;
			qf::qmlwidgets::dialogs::Dialog dlg(this->m_partWidget);
			//dlg.setButtons(QDialogButtonBox::Save);
			dlg.setCentralWidget(w);
			dlg.exec();
		});
	}
}

void RunsPlugin::onEditStartListRequest(int stage_id, int class_id, int competitor_id)
{
	//qf::qmlwidgets::dialogs::MessageBox::showError(nullptr, "Not implemented yet.");
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	if(!fwk->setActivePart("Runs"))
		return;
	auto *rw = partWidget()->findChild<RunsWidget*>();
	if(!rw)
		return;
	eventPlugin()->setCurrentStageId(stage_id);
	rw->editStartList(class_id, competitor_id);
}

int RunsPlugin::courseForRun(int run_id)
{
	// TODO: implementation should be dependend on event type and exposed to QML
	// see: CardCheck pattern
	return courseForRun_Classic(run_id);
}

int RunsPlugin::courseForRun_Classic(int run_id)
{
	qfs::QueryBuilder qb;
	qb.select("classdefs.courseId")
			.from("runs")
			.join("runs.competitorId", "competitors.id")
			.joinRestricted("competitors.classId", "classdefs.classId", "classdefs.stageId=runs.stageId")
			.where("runs.id=" QF_IARG(run_id));
	qfs::Query q;
	q.exec(qb.toString(), qf::core::Exception::Throw);
	int cnt = 0;
	int ret = 0;
	while (q.next()) {
		if(cnt > 0) {
			qfError() << "more courses found for run_id:" << run_id;
			return 0;
		}
		ret = q.value(0).toInt();
		cnt++;
	}
	return ret;
}


