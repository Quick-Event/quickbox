#include "speaker.h"

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/dialogs/messagebox.h>
#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/menubar.h>

#include <qf/core/sql/query.h>
#include <qf/core/log.h>

namespace qff = qf::qmlwidgets::framework;
namespace qfw = qf::qmlwidgets;

Speaker::Speaker(QObject *parent)
	: Super(parent), qf::qmlwidgets::framework::IPersistentSettings(this)
{
	setPersistentSettingsId("Speaker");
	connect(this, &Speaker::installed, this, &Speaker::onInstalled, Qt::QueuedConnection);
}

void Speaker::onInstalled()
{
	qfLogFuncFrame();
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	auto *a = new qfw::Action("Vacuum Database");
	fwk->menuBar()->actionForPath("tools")->addActionInto(a);
	//connect(a, &qfw::Action::triggered, this, &Speaker::doDbVacuum);
}

}
