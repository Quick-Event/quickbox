#include "examplecppqml.h"

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/dialogs/messagebox.h>
#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/menubar.h>

#include <qf/core/sql/query.h>
#include <qf/core/log.h>

namespace qff = qf::qmlwidgets::framework;
namespace qfw = qf::qmlwidgets;

ExampleCppQml::ExampleCppQml(QObject *parent)
	: Super(parent), qf::qmlwidgets::framework::IPersistentSettings(this)
{
	setPersistentSettingsId("ExampleCppQml");
	connect(this, &ExampleCppQml::installed, this, &ExampleCppQml::onInstalled, Qt::QueuedConnection);
}

void ExampleCppQml::onInstalled()
{
	qfLogFuncFrame();
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	auto *a = new qfw::Action("Vacuum Database");
	fwk->menuBar()->actionForPath("tools")->addActionInto(a);
	connect(a, &qfw::Action::triggered, this, &ExampleCppQml::doDbVacuum);
}

void ExampleCppQml::doDbVacuum()
{
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	if(!qfw::dialogs::MessageBox::askYesNo(fwk, tr("Process VACUUM on current event database?")))
		return;
	qf::core::sql::Query q;
	try {
		q.exec("VACUUM", qf::core::Exception::Throw);
		qfw::dialogs::MessageBox::showInfo(fwk, tr("VACUUM on current event database processed successfully"));
	}
	catch (qf::core::Exception &e) {
		qfw::dialogs::MessageBox::showException(fwk, e);
	}
}
