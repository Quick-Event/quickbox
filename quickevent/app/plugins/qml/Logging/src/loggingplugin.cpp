#include "loggingplugin.h"

#include <qf/core/utils/settings.h>

#include "loggerwidget.h"

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/framework/dockwidget.h>
#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/menubar.h>

namespace qfu = qf::core::utils;
namespace qff = qf::qmlwidgets::framework;
namespace qfw = qf::qmlwidgets;

LoggingPlugin::LoggingPlugin(QObject *parent)
	: Super(parent), qf::qmlwidgets::framework::IPersistentSettings(this)
{
	setPersistentSettingsId("LoggingPlugin");
	connect(this, &LoggingPlugin::installed, this, &LoggingPlugin::onInstalled, Qt::QueuedConnection);
}

void LoggingPlugin::onInstalled()
{
	qff::MainWindow *fwk = qff::MainWindow::frameWork();

	connect(fwk, &qff::MainWindow::aboutToClose, this, &LoggingPlugin::saveSettings);

	qfw::Action *a = new qfw::Action("Show application log");
	a->setShortcut("ctrl+L");
	//fwk->menuBar()->actionForPath("tools/pluginSettings")->addActionInto(actConfigureLogging);
	fwk->menuBar()->actionForPath("view")->addActionInto(a);
	connect(a, &qfw::Action::triggered, [this](bool)
	{
		this->setLogDockVisible(true);
	});
	loadSettings();
}

void LoggingPlugin::saveSettings()
{
	QString path = persistentSettingsPath();
	qfLogFuncFrame() << path;
	if(!path.isEmpty()) {
		qfu::Settings settings;
		settings.beginGroup(path + "/ui/docks/Logger");
		settings.setValue("visible", m_logDockWidget? m_logDockWidget->isVisible(): false);
	}
}

void LoggingPlugin::loadSettings()
{
	QString path = persistentSettingsPath();
	qfLogFuncFrame() << path;
	if(!path.isEmpty()) {
		qfu::Settings settings;
		settings.beginGroup(path + "/ui/docks/Logger");
		bool visible = settings.value("visible", false).toBool();
		setLogDockVisible(visible);
	}
}

void LoggingPlugin::setLogDockVisible(bool on)
{
	if(on && !m_logDockWidget) {
		m_logDockWidget = new qff::DockWidget(nullptr);
		m_logDockWidget->setObjectName("logDockWidget");
		m_logDockWidget->setWidget(new LoggerWidget());
		qff::MainWindow *fwk = qff::MainWindow::frameWork();
		fwk->addDockWidget(Qt::BottomDockWidgetArea, m_logDockWidget);
	}
	if(m_logDockWidget)
		m_logDockWidget->setVisible(on);
}

