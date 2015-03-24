#include "runsplugin.h"
#include "thispartwidget.h"

//#include <EventPlugin/eventplugin.h>

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/framework/dockwidget.h>
#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/menubar.h>

//#include <qf/core/log.h>

#include <QQmlEngine>

namespace qfw = qf::qmlwidgets;
namespace qff = qf::qmlwidgets::framework;
//namespace qfd = qf::qmlwidgets::dialogs;
//namespace qfs = qf::core::sql;

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
	/*
	{
		qfw::Action *a = new qfw::Action("Show registrations");
		a->setCheckable(true);
		a->setShortcut("ctrl+shift+R");
		//fwk->menuBar()->actionForPath("tools/pluginSettings")->addActionInto(actConfigureLogging);
		fwk->menuBar()->actionForPath("view")->addActionInto(a);
		connect(a, &qfw::Action::triggered, this, &RunsPlugin::setRegistrationsDockVisible);
	}
	*/
	emit nativeInstalled();
}


