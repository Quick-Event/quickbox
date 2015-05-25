#include "loggingplugin.h"

#include <qf/core/utils/settings.h>

#include "../loggerwidget.h"

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/framework/dockwidget.h>
#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/menubar.h>

namespace qfu = qf::core::utils;
namespace qff = qf::qmlwidgets::framework;
namespace qfw = qf::qmlwidgets;

using namespace Logging;

LoggingPlugin::LoggingPlugin(QObject *parent)
	: Super(parent), qf::qmlwidgets::framework::IPersistentSettings(this)
{
	setPersistentSettingsId("LoggingPlugin");
	connect(this, &LoggingPlugin::installed, this, &LoggingPlugin::onInstalled, Qt::QueuedConnection);
}

void LoggingPlugin::setLogDockVisible(bool b)
{
	m_logDockWidget->setVisible(b);
}

void LoggingPlugin::onInstalled()
{
	qff::MainWindow *fwk = qff::MainWindow::frameWork();

	connect(fwk, &qff::MainWindow::aboutToClose, this, &LoggingPlugin::saveSettings);

	{
		m_logDockWidget = new qff::DockWidget(nullptr);
		m_logDockWidget->setObjectName("loggingDockWidget");
		m_logDockWidget->setWindowTitle(tr("Application log"));
		fwk->addDockWidget(Qt::BottomDockWidgetArea, m_logDockWidget);
		auto *w = new LoggerWidget();
		m_logDockWidget->setWidget(w);
		m_logDockWidget->hide();
		connect(m_logDockWidget, &qff::DockWidget::visibleChanged, this, &LoggingPlugin::onLogDockVisibleChanged);
	}
	{
		auto *a = m_logDockWidget->toggleViewAction();
		//a->setCheckable(true);
		a->setShortcut(QKeySequence("ctrl+shift+L"));
		fwk->menuBar()->actionForPath("view")->addActionInto(a);
	}
	loadSettings();
}

void LoggingPlugin::saveSettings()
{
	QString path = persistentSettingsPath();
	qfLogFuncFrame() << path;
	/*
	if(!path.isEmpty()) {
		qfu::Settings settings;
		settings.beginGroup(path + "/ui/docks/Logger");
		settings.setValue("visible", m_logDockWidget? m_logDockWidget->isVisible(): false);
	}
	*/
}

void LoggingPlugin::loadSettings()
{
	QString path = persistentSettingsPath();
	qfLogFuncFrame() << path;
	/*
	if(!path.isEmpty()) {
		qfu::Settings settings;
		settings.beginGroup(path + "/ui/docks/Logger");
		bool visible = settings.value("visible", false).toBool();
		onLogDockVisibleChanged(visible);
	}
	*/
}

void LoggingPlugin::onLogDockVisibleChanged(bool on)
{
	if(on) {
		//w->reload();
		//m_logDockWidget->loadPersistentSettingsRecursively();
	}
}

