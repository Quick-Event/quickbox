#include "runsplugin.h"
#include "thispartwidget.h"
#include "drawing/drawingganttwidget.h"

//#include <EventPlugin/eventplugin.h>

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/framework/dockwidget.h>
#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/menubar.h>

#include <qf/core/log.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/qmlwidgets/dialogs/dialog.h>

#include <QQmlEngine>

namespace qfw = qf::qmlwidgets;
namespace qff = qf::qmlwidgets::framework;
//namespace qfd = qf::qmlwidgets::dialogs;
namespace qfs = qf::core::sql;

using namespace Runs;

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

	emit nativeInstalled();

	auto *a_draw = m_partWidget->menuBar()->actionForPath("drawing");
	a_draw->setText("&Draw");
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
	{
		qfw::Action *a = new qfw::Action("Draw current class");
		//a->setShortcut("ctrl+shift+R");
		a_draw->addActionInto(a);
		//connect(a, &qfw::Action::triggered, this, &RunsPlugin::setRegistrationsDockVisible);
	}
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


