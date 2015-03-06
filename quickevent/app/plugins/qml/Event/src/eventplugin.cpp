#include "eventplugin.h"
#include "connectdbdialogwidget.h"

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/dialogs/dialog.h>
#include <qf/qmlwidgets/dialogs/messagebox.h>
#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/menubar.h>

#include <qf/core/log.h>

#include <QSqlDatabase>
#include <QSqlError>

namespace qfw = qf::qmlwidgets;
namespace qff = qf::qmlwidgets::framework;
namespace qfd = qf::qmlwidgets::dialogs;

EventPlugin::EventPlugin(QObject *parent)
	: Super(parent)
{
	connect(this, &EventPlugin::installed, this, &EventPlugin::onInstalled, Qt::QueuedConnection);
}

void EventPlugin::onInstalled()
{
	qff::MainWindow *fwk = qff::MainWindow::frameWork();

	qfw::Action *a_connect = new qfw::Action("&Connect to database");
	//a->setShortcut("ctrl+L");
	fwk->menuBar()->actionForPath("view")->addActionInto(a_connect);
	connect(a_connect, &qfw::Action::triggered, [this](bool)
	{
		this->connectToSqlServer();
	});

	qfw::Action *a_quit = fwk->menuBar()->actionForPath("file/quit", false);
	a_quit->addActionBefore(a_connect);
	a_quit->addSeparatorBefore();

	connect(fwk, &qff::MainWindow::pluginsLoaded, [this]
	{
		this->connectToSqlServer();
	});
}

void EventPlugin::connectToSqlServer()
{
	qfLogFuncFrame();

	bool connect_ok = false;
	ConnectionType connection_type = ConnectionType::SingleFile;

	qfd::Dialog dlg;
	dlg.setButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	ConnectdbDialogWidget *conn_w = new ConnectdbDialogWidget();
	dlg.setCentralWidget(conn_w);
	while(!connect_ok) {
		conn_w->loadSettings();
		if(!dlg.exec())
			break;

		conn_w->saveSettings();
		connection_type = conn_w->connectionType();
		qfDebug() << "connection_type:" << (int)connection_type;
		QString driver_name = "QSQLITE";
		if(connection_type == ConnectionType::SqlServer) {
			driver_name = "QPSQL";
		}
		QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
		QSqlDatabase db = QSqlDatabase::addDatabase(driver_name);
		connect_ok = db.isValid();
		qfInfo() << "Adding database driver:" << driver_name << "OK:" << connect_ok;
		if(connect_ok) {
			//Log.info(db, db.connectionName, db.driverName);
			//settings.beginGroup(connection_type);
			if(connection_type == ConnectionType::SqlServer) {
				db.setHostName(conn_w->serverHost());
				db.setPort(conn_w->serverPort());
				db.setUserName(conn_w->serverUser());
				db.setPassword(conn_w->serverPassword());
				db.setDatabaseName("quickevent");
				qfInfo().nospace() << "connecting to: " << db.userName() << "@" << db.hostName() << ":" << db.port();
				connect_ok = db.open();
			}
			else if(connection_type == ConnectionType::SingleFile) {
			}
		}
		if(!connect_ok) {
			qff::MainWindow *fwk = qff::MainWindow::frameWork();
			qfd::MessageBox::showError(fwk, tr("Connect Database Error: %1").arg(db.lastError().text()));
		}
	}
	//Log.info("settinmg API connection type to:", connection_type);
	//root.api.connectionType = connection_type;
	//root.api.sqlServerConnected = connect_ok;
}

