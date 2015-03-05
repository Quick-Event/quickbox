#include "loggingplugin.h"

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/menubar.h>

namespace qff = qf::qmlwidgets::framework;
namespace qfw = qf::qmlwidgets;

LoggingPlugin::LoggingPlugin(QObject *parent)
	: Super(parent)
{
	connect(this, &LoggingPlugin::installed, this, &LoggingPlugin::onInstalled, Qt::QueuedConnection);
}

void LoggingPlugin::onInstalled()
{
	qff::MainWindow *fwk = qff::MainWindow::frameWork();

	connect(fwk, &qff::MainWindow::aboutToClose, this, &LoggingPlugin::saveSettings);

	qfw::Action *actShowLogView = new qfw::Action("Show application log");
	//fwk->menuBar()->actionForPath("tools/pluginSettings")->addActionInto(actConfigureLogging);
	fwk->menuBar()->actionForPath("view")->addActionInto(actShowLogView);
/*
	var core_feature = FrameWork.plugin("Core");
	var settings = core_feature.api.createSettings();
	settings.beginGroup("persistentSettings/ui/docks/Logger");
	var dock_visible = settings.value('visible');
	settings.destroy();
	//console.debug("logger dock visible:", dock_visible, typeof dock_visible);
	//showLogDockWidget(dock_visible);
	*/
}

void LoggingPlugin::saveSettings()
{

}

