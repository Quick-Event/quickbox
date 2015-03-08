#include "eventplugin.h"
#include "connectdbdialogwidget.h"
#include "connectionsettings.h"

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/dialogs/dialog.h>
#include <qf/qmlwidgets/dialogs/messagebox.h>
#include <qf/qmlwidgets/dialogs/filedialog.h>
#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/menubar.h>

#include <qf/core/log.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/core/sql/connection.h>

#include <QInputDialog>
#include <QSqlDatabase>
#include <QSqlError>

namespace qfw = qf::qmlwidgets;
namespace qff = qf::qmlwidgets::framework;
namespace qfd = qf::qmlwidgets::dialogs;
namespace qfs = qf::core::sql;

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
	connect(a_connect, SIGNAL(triggered()), this, SLOT(connectToSqlServer()));

	qfw::Action *a_open = new qfw::Action("&Open event");
	a_open->setShortcut("Ctrl+O");
	connect(a_open, SIGNAL(triggered()), this, SLOT(openEvent()));

	connect(fwk, &qff::MainWindow::pluginsLoaded, this, &EventPlugin::connectToSqlServer);

	qfw::Action *a_quit = fwk->menuBar()->actionForPath("file/quit", false);
	a_quit->addActionBefore(a_connect);
	a_quit->addSeparatorBefore();
	a_connect->addActionAfter(a_open);
}

void EventPlugin::connectToSqlServer()
{
	qfLogFuncFrame();

	qff::MainWindow *fwk = qff::MainWindow::frameWork();

	bool connect_ok = false;
	ConnectionType connection_type = ConnectionType::SingleFile;

	qfd::Dialog dlg(fwk);
	dlg.setButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	ConnectDbDialogWidget *conn_w = new ConnectDbDialogWidget();
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

static QString eventNameToFileName(const QString &event_name)
{
	ConnectionSettings connection_settings;
	QString ret = connection_settings.singleWorkingDir() + '/' + event_name + ".qbe";
	return ret;
}

bool EventPlugin::openEvent(const QString &_event_name)
{
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	QString event_name = _event_name;
	ConnectionSettings connection_settings;
	ConnectionType connection_type = connection_settings.connectionType();
	//console.debug("openEvent()", "event_name:", event_name, "connection_type:", connection_type);
	bool ok = false;
	if(connection_type == ConnectionType::SqlServer) {
		//console.debug(db);
		qfs::Connection conn;
		qfs::Query q(conn);
		qfs::QueryBuilder qb;
		if(event_name.isEmpty()) {
			qb.select("nspname")
					.from("pg_catalog.pg_namespace  AS n")
				.where("nspname NOT LIKE 'pg\\_%'")
				.where("nspname NOT IN ('public', 'information_schema')")
				.orderBy("nspname");
			q.exec(qb.toString());
			QStringList events;
			while(q.next()) {
				events << q.value("nspname").toString();
			}
			event_name = QInputDialog::getItem(fwk, tr("Query"), tr("Open event"), events, 0, false);
		}
		//Log.info(event_name, typeof event_name, (event_name)? "T": "F");
		if(!event_name.isEmpty()) {
			if(conn.setCurrentSchema(event_name)) {
				ConnectionSettings settings;
				settings.setEventName(event_name);
				ok = true;
			}
		}
	}
	else if(connection_type == ConnectionType::SingleFile) {
		QString event_fn;
		if(!event_name.isEmpty()) {
			event_fn = eventNameToFileName(event_name);
		}
		else {
			event_fn = qfd::FileDialog::getOpenFileName(fwk, tr("Select event"), connection_settings.singleWorkingDir(), tr("Quick Event files (*.qbe)"));
			if(!event_fn.isEmpty()) {
				event_fn.replace("\\", "/");
				int ix = event_fn.lastIndexOf("/");
				event_name = event_fn.mid(ix + 1, event_fn.length() - 4);
				QString working_dir = event_fn.mid(0, ix);
				if(!event_name.isEmpty()) {
					connection_settings.setEventName(event_name);
					connection_settings.setSingleWorkingDir(working_dir);
				}
			}
		}
		//Log.info(event_name, typeof event_name, (event_name)? "T": "F");
		if(!event_fn.isEmpty()) {
			{
				qfs::Connection conn;
				conn.close();
				QSqlDatabase::removeDatabase(conn.connectionName());
			}
			qfs::Connection conn;
			conn.setDatabaseName(event_fn);
			qfInfo() << "Opening database file";
			if(conn.open()) {
				qfs::Query q(conn);
				ok = q.exec("PRAGMA foreign_keys=ON");
			}
			else {
				qfd::MessageBox::showError(fwk, tr("Open Database Error: %1").arg(conn.errorString()));
			}
		}
	}
	else {
		qfError() << "Invalid connection type:" << static_cast<int>(connection_type);
	}
	if(ok) {
		//root.currentEventName = event_name;
		//FrameWork.plugin('Event').api.config.reload();
	}
	return ok;
}

