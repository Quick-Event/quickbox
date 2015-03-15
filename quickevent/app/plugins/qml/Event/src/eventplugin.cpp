#include "eventplugin.h"
#include "connectdbdialogwidget.h"
#include "connectionsettings.h"
#include "eventdialogwidget.h"

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/dialogs/dialog.h>
#include <qf/qmlwidgets/dialogs/messagebox.h>
#include <qf/qmlwidgets/dialogs/filedialog.h>
#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/menubar.h>
#include <qf/qmlwidgets/statusbar.h>
#include <qf/qmlwidgets/toolbar.h>

#include <qf/core/log.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/core/sql/connection.h>
#include <qf/core/sql/transaction.h>

#include <QInputDialog>
#include <QSqlDatabase>
#include <QSqlError>
#include <QComboBox>
#include <QLabel>
#include <QMetaObject>

namespace qfw = qf::qmlwidgets;
namespace qff = qf::qmlwidgets::framework;
namespace qfd = qf::qmlwidgets::dialogs;
namespace qfs = qf::core::sql;

static QString eventNameToFileName(const QString &event_name)
{
	ConnectionSettings connection_settings;
	QString ret = connection_settings.singleWorkingDir() + '/' + event_name + ".qbe";
	return ret;
}

int EventPlugin::dlTest = 0;

EventPlugin::EventPlugin(QObject *parent)
	: Super(parent)
{
	connect(this, &EventPlugin::installed, this, &EventPlugin::onInstalled);//, Qt::QueuedConnection);
}

Event::EventConfig *EventPlugin::eventConfig(bool reload)
{
	if(m_eventConfig == nullptr) {
		m_eventConfig = new Event::EventConfig(this);
		reload = true;
	}
	if(reload) {
		m_eventConfig->load();
		emit eventNameChanged(m_eventConfig->eventName());
	}
	return m_eventConfig;
}

void EventPlugin::onInstalled()
{
	qff::MainWindow *fwk = qff::MainWindow::frameWork();

	m_actConnectDb = new qfw::Action("&Connect to database");
	//a->setShortcut("ctrl+L");
	connect(m_actConnectDb, SIGNAL(triggered()), this, SLOT(connectToSqlServer()));

	m_actOpenEvent = new qfw::Action("&Open event");
	m_actOpenEvent->setShortcut("Ctrl+O");
	m_actOpenEvent->setEnabled(false);
	connect(m_actOpenEvent, SIGNAL(triggered()), this, SLOT(openEvent()));

	m_actCreateEvent = new qfw::Action("&Create event");
	m_actCreateEvent->setShortcut("Ctrl+N");
	m_actCreateEvent->setEnabled(false);
	connect(m_actCreateEvent, SIGNAL(triggered()), this, SLOT(createEvent()));

	//QObject *core_plugin = fwk->plugin("Core", qf::core::Exception::Throw);
	connect(this, SIGNAL(eventNameChanged(QString)), fwk->statusBar(), SLOT(setEventName(QString)));
	connect(this, SIGNAL(currentStageChanged(int)), fwk->statusBar(), SLOT(setStageNo(int)));
	connect(fwk, &qff::MainWindow::pluginsLoaded, this, &EventPlugin::connectToSqlServer);

	qfw::Action *a_quit = fwk->menuBar()->actionForPath("file/quit", false);
	a_quit->addActionBefore(m_actConnectDb);
	a_quit->addSeparatorBefore();
	m_actConnectDb->addActionAfter(m_actCreateEvent);
	m_actCreateEvent->addActionAfter(m_actOpenEvent);

	qfw::ToolBar *tb = fwk->toolBar("Event", true);
	tb->setObjectName("EventToolbar");
	QComboBox *cbx_stage = new QComboBox();
	for (int i = 0; i < 5; ++i) {
		cbx_stage->addItem("E" + QString::number(i + 1), i + 1);
	}
	tb->addWidget(new QLabel(tr("Stage ")));
	tb->addWidget(cbx_stage);

	setCurrentStage(1);
}

void EventPlugin::connectToSqlServer()
{
	qfLogFuncFrame();

	qff::MainWindow *fwk = qff::MainWindow::frameWork();

	bool connect_ok = false;
	ConnectionType connection_type = ConnectionType::SingleFile;

	qfd::Dialog dlg(fwk);
	dlg.setButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	dlg.setSavePersistentPosition(false);
	ConnectDbDialogWidget *conn_w = new ConnectDbDialogWidget();
	dlg.setCentralWidget(conn_w);
	while(!connect_ok) {
		conn_w->loadSettings();
		if(!dlg.exec())
			break;

		conn_w->saveSettings();
		connection_type = conn_w->connectionType();
		qfDebug() << "connection_type:" << (int)connection_type;
		if(connection_type == ConnectionType::SqlServer) {
			QString driver_name = "QPSQL";
			QSqlDatabase::removeDatabase(QSqlDatabase::defaultConnection);
			QSqlDatabase db = QSqlDatabase::addDatabase(driver_name);
			connect_ok = db.isValid();
			qfInfo() << "Adding database driver:" << driver_name << "OK:" << connect_ok;
			if(connect_ok) {
				//Log.info(db, db.connectionName, db.driverName);
				db.setHostName(conn_w->serverHost());
				db.setPort(conn_w->serverPort());
				db.setUserName(conn_w->serverUser());
				db.setPassword(conn_w->serverPassword());
				db.setDatabaseName("quickevent");
				qfInfo().nospace() << "connecting to: " << db.userName() << "@" << db.hostName() << ":" << db.port();
				connect_ok = db.open();
			}
			if(!connect_ok) {
				qff::MainWindow *fwk = qff::MainWindow::frameWork();
				qfd::MessageBox::showError(fwk, tr("Connect Database Error: %1").arg(db.lastError().text()));
			}
		}
		else {
			connect_ok = true;
		}
	}
	setDbOpen(connect_ok);
	m_actCreateEvent->setEnabled(connect_ok);
	m_actOpenEvent->setEnabled(connect_ok);
	if(connect_ok) {
		openEvent(conn_w->eventName());
	}
}

bool EventPlugin::createEvent(const QVariantMap &event_params)
{
	bool ok = false;
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	qfd::Dialog dlg(fwk);
	dlg.setButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	EventDialogWidget *event_w = new EventDialogWidget();
	event_w->loadParams(event_params);
	dlg.setCentralWidget(event_w);
	if(!dlg.exec())
		return false;
	QVariantMap new_params = event_w->saveParams();
	Event::EventConfig event_config;
	ConnectionSettings connection_settings;
	event_config.setValues(new_params);
	int stage_count = event_params.value("event.stageCount").toInt();
	if(stage_count == 0)
		stage_count = event_config.value("event.stageCount").toInt();
	qfInfo() << "createEvent, stage_count:" << stage_count;
	QF_ASSERT(stage_count > 0, "Stage count have to be greater than 0", return false);

	QString event_name = event_config.value("event.name").toString();
	qfInfo() << "will create:" << event_name;
	EventPlugin::ConnectionType connection_type = connection_settings.connectionType();
	qfs::Connection conn = qfs::Connection::forName();
	//QF_ASSERT(conn.isOpen(), "Connection is not open", return false);
	if(connection_type == ConnectionType::SingleFile) {
		QString event_fn = eventNameToFileName(event_name);
		conn.close();
		conn.setDatabaseName(event_fn);
		conn.open();
	}
	if(conn.isOpen()) {
		QVariantMap create_options;
		create_options["schemaName"] = event_name;
		create_options["driverName"] = conn.driverName();

		QVariant ret_val;
		QMetaObject::invokeMethod(this, "createDbSqlScript", Qt::DirectConnection,
								  Q_RETURN_ARG(QVariant, ret_val),
								  Q_ARG(QVariant, create_options));
		QStringList create_script = ret_val.toStringList();

		qfInfo().nospace() << create_script.join(";\n") << ';';
		qfs::Query q(conn);
		do {
			qfs::Transaction transaction(conn);
			for(auto cmd : create_script) {
				if(cmd.isEmpty())
					continue;
				if(cmd.startsWith(QLatin1String("--")))
					continue;
				qfDebug() << cmd << ';';
				ok = q.exec(cmd);
				if(!ok) {
					qfInfo() << cmd;
					qfError() << q.lastError().text();
					break;
				}
			}
			if(!ok)
				break;
			qfDebug() << "creating stages:" << stage_count;
			QString stage_table_name = "stages";
			if(connection_type == ConnectionType::SqlServer)
				stage_table_name = event_name + '.' + stage_table_name;
			q.prepare("INSERT INTO " + stage_table_name + " (id) VALUES (:id)");
			for(int i=0; i<stage_count; i++) {
				q.bindValue(":id", i+1);
				ok = q.exec();
				if(!ok) {
					break;
				}
			}
			if(!ok)
				break;
			transaction.commit();
		} while(false);
		if(ok) {
			event_config.save();
		}
		else {
			qfd::MessageBox::showError(fwk, tr("Create Database Error: %1").arg(q.lastError().text()));
		}
	}

	return ok;

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
		qfs::Connection conn(QSqlDatabase::database());
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
				qfs::Connection conn(QSqlDatabase::database());
				conn.close();
				qfInfo() << "removing database:" << conn.connectionName();
				QSqlDatabase::removeDatabase(conn.connectionName());
			}
			QSqlDatabase::addDatabase("QSQLITE");
			qfs::Connection conn(QSqlDatabase::database());
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
		eventConfig()->setEventName(event_name);
		emit reloadDataRequest();
	}
	return ok;
}

void EventPlugin::setDbOpen(bool ok)
{
	if(ok != m_dbOpen) {
		m_dbOpen = ok;
		emit dbOpenChanged(ok);
	}
}

