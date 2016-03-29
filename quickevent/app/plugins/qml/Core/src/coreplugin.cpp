#include "coreplugin.h"
#include "widgets/appstatusbar.h"

#include <qf/qmlwidgets/framework/mainwindow.h>

#include <qf/core/sql/connection.h>
#include <qf/core/log.h>

#include <QCoreApplication>
#include <QProcess>
#include <QMessageBox>

namespace qff = qf::qmlwidgets::framework;

CorePlugin::CorePlugin(QObject *parent)
	: Super(parent)
{
	connect(this, &CorePlugin::installed, this, &CorePlugin::onInstalled);//, Qt::QueuedConnection);
}

void CorePlugin::onInstalled()
{
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	fwk->setStatusBar(new AppStatusBar());


}

void CorePlugin::launchSqlTool()
{
	qfLogFuncFrame();
	QString program = QCoreApplication::applicationDirPath() + "/qsqlmon";
#ifdef Q_OS_WIN
	program += ".exe";
#endif
	qfDebug() << "launchnig" << program;
	QStringList otcs;
	{
		auto conn = qf::core::sql::Connection::forName();
		otcs << "description=QuickEvent";
		otcs << "driver=" + conn.driverName();
		otcs << "host=" + conn.hostName();
		otcs << "port=" + QString::number(conn.port());
		otcs << "user=" + conn.userName();
		otcs << "password=" + conn.password();
		otcs << "database=" + conn.databaseName();
	}

	QStringList arguments;
	arguments << "--one-time-connection-settings" << otcs.join('&');
	QProcess *process = new QProcess(this);
	process->start(program, arguments);
}

void CorePlugin::aboutQuickEvent()
{
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	QString version_string;
	QMetaObject::invokeMethod(fwk, "versionString", Qt::DirectConnection
							  , Q_RETURN_ARG(QString, version_string));
	QMessageBox::about(fwk
					   , tr("About Quick Event")
					   , tr("The <b>Quick Event</b> is an application which helps you to organize the orienteering events."
							"<br/><br/>"
							"version: %1"
							"<br/>"
							"build: %2 %3"
							).arg(version_string).arg(__DATE__).arg(__TIME__)
					   );
}

void CorePlugin::aboutQt()
{
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	QMessageBox::aboutQt(fwk , tr("About Qt"));
}

