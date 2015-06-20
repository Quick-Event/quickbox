#include "eventplugin.h"
#include "../connectdbdialogwidget.h"
#include "../connectionsettings.h"
#include "../eventdialogwidget.h"
#include "stagedocument.h"
#include "stagewidget.h"

#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/dialogs/dialog.h>
#include <qf/qmlwidgets/dialogs/messagebox.h>
#include <qf/qmlwidgets/dialogs/filedialog.h>
#include <qf/qmlwidgets/action.h>
#include <qf/qmlwidgets/menubar.h>
#include <qf/qmlwidgets/statusbar.h>
#include <qf/qmlwidgets/toolbar.h>
#include <qf/qmlwidgets/style.h>

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
#include <QSqlDriver>
#include <QJsonObject>

namespace qfw = qf::qmlwidgets;
namespace qff = qf::qmlwidgets::framework;
namespace qfd = qf::qmlwidgets::dialogs;
namespace qfs = qf::core::sql;

using namespace Event;

static auto QBE_EXT = QStringLiteral(".qbe");

static QString eventNameToFileName(const QString &event_name)
{
	ConnectionSettings connection_settings;
	QString ret = connection_settings.singleWorkingDir() + '/' + event_name + QBE_EXT;
	return ret;
}

static QString fileNameToEventName(const QString &file_name)
{
	QString fn = file_name;
	fn.replace("\\", "/");
	int ix = fn.lastIndexOf("/");
	QString event_name = fn.mid(ix + 1);
	if(event_name.endsWith(QBE_EXT, Qt::CaseInsensitive))
		event_name = event_name.mid(0, event_name.length() - QBE_EXT.length());
	return event_name;
}

/// strange is that 'quickboxDbEvent' just doesn't work without any error
/// from psql doc: Commonly, the channel name is the same as the name of some table in the database
/// I guess that channel name cannot contain capital letters to work
const char *EventPlugin::DBEVENT_NOTIFY_NAME = "quickbox_db_event";

EventPlugin::EventPlugin(QObject *parent)
	: Super(parent)
{
	connect(this, &EventPlugin::installed, this, &EventPlugin::onInstalled);//, Qt::QueuedConnection);
}

void EventPlugin::initEventConfig()
{
	if(m_eventConfig == nullptr) {
		m_eventConfig = new Event::EventConfig(this);
	}
	else {
		qfWarning() << "Event config exists already!";
	}
}

Event::EventConfig *EventPlugin::eventConfig(bool reload)
{
	if(m_eventConfig == nullptr) {
		m_eventConfig = new Event::EventConfig(this);
		reload = true;
	}
	if(reload) {
		m_eventConfig->load();
		//emit eventNameChanged(m_eventConfig->eventName());
	}
	return m_eventConfig;
}

void EventPlugin::setCurrentStageId(int stage_id)
{
	m_cbxStage->setCurrentIndex(stage_id - 1);
	emit currentStageIdChanged(stage_id);
}

int EventPlugin::currentStageId()
{
	return m_cbxStage->currentIndex() + 1;
}

StageData EventPlugin::stageData(int stage_id)
{
	QVariantMap ret;
	if(!m_stageCache.contains(stage_id)) {
		Event::StageDocument doc;
		doc.load(stage_id);
		StageData s(&doc);
		m_stageCache[stage_id] = s;
	}
	ret = m_stageCache.value(stage_id);
	return ret;
}

void EventPlugin::clearStageDataCache()
{
	m_stageCache.clear();
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

	m_actCreateEvent = new qfw::Action("Create eve&nt");
	m_actCreateEvent->setShortcut("Ctrl+N");
	m_actCreateEvent->setEnabled(false);
	connect(m_actCreateEvent, SIGNAL(triggered()), this, SLOT(createEvent()));

	m_actEditEvent = new qfw::Action("E&dit event");
	//m_actEditEvent->setShortcut("Ctrl+N");
	m_actEditEvent->setEnabled(false);
	connect(m_actEditEvent, SIGNAL(triggered()), this, SLOT(editEvent()));
	connect(this, &EventPlugin::eventNameChanged, [this](const QString &event_name) {
		this->m_actEditEvent->setEnabled(!event_name.isEmpty());
	});

	//QObject *core_plugin = fwk->plugin("Core", qf::core::Exception::Throw);
	connect(this, SIGNAL(eventNameChanged(QString)), fwk->statusBar(), SLOT(setEventName(QString)));
	connect(this, SIGNAL(currentStageIdChanged(int)), fwk->statusBar(), SLOT(setStageNo(int)));
	connect(fwk, &qff::MainWindow::pluginsLoaded, this, &EventPlugin::connectToSqlServer);
	connect(this, &EventPlugin::eventOpened, this, &EventPlugin::onEventOpened);

	qfw::Action *a_quit = fwk->menuBar()->actionForPath("file/import", false);
	a_quit->addActionBefore(m_actConnectDb);
	a_quit->addSeparatorBefore();
	m_actConnectDb->addActionAfter(m_actCreateEvent);
	m_actCreateEvent->addActionAfter(m_actOpenEvent);
	m_actOpenEvent->addActionAfter(m_actEditEvent);

	qfw::ToolBar *tb = fwk->toolBar("Event", true);
	tb->setObjectName("EventToolbar");
	tb->addWidget(new QLabel(tr("Stage ")));
	m_cbxStage = new QComboBox();
	connect(m_cbxStage, SIGNAL(activated(int)), this, SLOT(onCbxStageActivated(int)));
	tb->addWidget(m_cbxStage);


	auto *style = qf::qmlwidgets::Style::instance();
	QIcon ico(style->icon("settings"));
	m_actEditStage = new qfw::Action(ico, "Stage settings");
	//m_actOpenEvent->setEnabled(false);
	connect(m_actEditStage, SIGNAL(triggered()), this, SLOT(editStage()));
	tb->addAction(m_actEditStage);
}

void EventPlugin::onCbxStageActivated(int ix)
{
	emit this->currentStageIdChanged(ix + 1);
}

void EventPlugin::editStage()
{
	qfLogFuncFrame();// << "id:" << id << "mode:" << mode;
	int stage_id = currentStageId();
	if(stage_id < 0)
		return;
	Event::StageWidget *w = new Event::StageWidget();
	w->setWindowTitle(tr("Edit Stage"));
	auto fwk = qf::qmlwidgets::framework::MainWindow::frameWork();
	qfd::Dialog dlg(QDialogButtonBox::Save | QDialogButtonBox::Cancel, fwk);
	dlg.setDefaultButton(QDialogButtonBox::Save);
	dlg.setCentralWidget(w);
	w->load(stage_id);
	if(dlg.exec()) {
		clearStageDataCache();
	}
}

void EventPlugin::emitDbEvent(const QString &domain, const QVariant &payload, bool loopback)
{
	qfLogFuncFrame() << "domain:" << domain << "payload:" << payload;
	if(loopback) {
		// emit queued
		//emit dbEventNotify(domain, payload);
		QMetaObject::invokeMethod(this, "dbEventNotify", Qt::QueuedConnection,
								  Q_ARG(QString, domain),
								  Q_ARG(QVariant, payload));
	}
	if(connectionType() == ConnectionType::SingleFile)
		return;
	//QVariantMap m;
	QJsonObject jso;
	jso[QLatin1String("event")] = eventName();
	jso[QLatin1String("domain")] = domain;
	jso[QLatin1String("payload")] = QJsonValue::fromVariant(payload);
	QJsonDocument jsd(jso);
	QString payload_str = QString::fromUtf8(jsd.toJson(QJsonDocument::Compact));
	payload_str = qf::core::sql::Connection::escapeJsonForSql(payload_str);
	qf::core::sql::Connection conn = qf::core::sql::Connection::forName();
	QString qs = QString("NOTIFY ") + DBEVENT_NOTIFY_NAME + ", '" + payload_str + "'";
	qfDebug() << conn.driver() << "executing SQL:" << qs;
	QSqlQuery q(conn);
	if(!q.exec(qs)) {
		qfError() << "emitDbEventNotify Error:" << qs << q.lastError().text();
	}
}

void EventPlugin::onDbEvent(const QString &name, QSqlDriver::NotificationSource source, const QVariant &payload)
{
	qfLogFuncFrame() << "name:" << name << "source:" << source << "payload:" << payload;
	if(name == QLatin1String(DBEVENT_NOTIFY_NAME)) {
		if(source == QSqlDriver::OtherSource) {
			QJsonDocument jsd = QJsonDocument::fromJson(payload.toString().toUtf8());
			QVariantMap m = jsd.toVariant().toMap();
			QString domain = m.value(QStringLiteral("domain")).toString();
			if(domain.isEmpty()) {
				qfWarning() << "DbNotify with invalid domain, payload:" << payload.toString();
				return;
			}
			QString event_name = m.value(QStringLiteral("event")).toString();
			if(event_name.isEmpty()) {
				qfWarning() << "DbNotify with invalid event name, payload:" << payload.toString();
				return;
			}
			if(event_name == eventName()) {
				QVariant pl = m.value(QStringLiteral("payload"));
				qfDebug() << "emitting domain:" << domain << "payload:" << pl;
				emit dbEventNotify(domain, pl);
			}
		}
		else {
			//qfDebug() << "self db notify";
		}
	}
}

void EventPlugin::onEventOpened()
{
	qfLogFuncFrame() << "stage count:" << stageCount();
	m_cbxStage->blockSignals(true);
	m_cbxStage->clear();
	int stage_cnt = stageCount();
	for (int i = 0; i < stage_cnt; ++i) {
		m_cbxStage->addItem("E" + QString::number(i + 1), i + 1);
	}
	m_cbxStage->setCurrentIndex(0);
	m_cbxStage->blockSignals(false);
	emit this->currentStageIdChanged(currentStageId());
}

EventPlugin::ConnectionType EventPlugin::connectionType() const
{
	ConnectionSettings connection_settings;
	return connection_settings.connectionType();
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
				if(connect_ok) {
					bool ok = connect(db.driver(), SIGNAL(notification(QString,QSqlDriver::NotificationSource,QVariant)), this, SLOT(onDbEvent(QString,QSqlDriver::NotificationSource,QVariant)));
					if(ok)
						ok = db.driver()->subscribeToNotification(DBEVENT_NOTIFY_NAME);
					if(!ok)
						qfError() << "Failed to subscribe db notification:" << DBEVENT_NOTIFY_NAME;
					else {
						qfInfo() << "Successfully subscribe db notification:" << DBEVENT_NOTIFY_NAME;
						qfInfo() << db.driver() << "subscribedToNotifications:" << db.driver()->subscribedToNotifications().join(", ");
					}
				}
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

bool EventPlugin::createEvent(const QString &_event_name, const QVariantMap &event_params)
{
	qfLogFuncFrame();
	closeEvent();
	bool ok = false;
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	qfd::Dialog dlg(fwk);
	dlg.setButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	EventDialogWidget *event_w = new EventDialogWidget();
	event_w->setEventId(_event_name);
	event_w->loadParams(event_params);
	dlg.setCentralWidget(event_w);
	if(!dlg.exec())
		return false;

	QString event_name = event_w->eventId();
	if(event_name.isEmpty()) {
		qf::qmlwidgets::dialogs::MessageBox::showError(fwk, tr("Event ID cannot be empty."));
		return false;
	}

	QVariantMap new_params = event_w->saveParams();
	Event::EventConfig event_config;
	//ConnectionSettings connection_settings;
	event_config.setValue("event", new_params);
	int stage_count = event_params.value("stageCount").toInt();
	if(stage_count == 0)
		stage_count = event_config.stageCount();
	qfInfo() << "createEvent, stage_count:" << stage_count;
	QF_ASSERT(stage_count > 0, "Stage count have to be greater than 0", return false);

	qfInfo() << "will create:" << event_name;
	EventPlugin::ConnectionType connection_type = connectionType();
	qfs::Connection conn = qfs::Connection::forName();
	//QF_ASSERT(conn.isOpen(), "Connection is not open", return false);
	if(connection_type == ConnectionType::SingleFile) {
		QString event_fn = eventNameToFileName(event_name);
		conn.close();
		conn.setDatabaseName(event_fn);
		conn.open();
	}
	//else {
	//	conn.setCurrentSchema(event_name);
	//}
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
			conn.setCurrentSchema(event_name);
			event_config.save();
			transaction.commit();
		} while(false);
		if(!ok) {
			qfd::MessageBox::showError(fwk, tr("Create Database Error: %1").arg(q.lastError().text()));
		}
	}
	else {
		qfd::MessageBox::showError(fwk, tr("Cannot create event, database is not open: %1").arg(conn.lastError().text()));
	}
	if(ok) {
		ok = openEvent(event_name);
	}
	return ok;	
}

void EventPlugin::editEvent()
{
	qfLogFuncFrame();
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	qfd::Dialog dlg(fwk);
	dlg.setButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
	EventDialogWidget *event_w = new EventDialogWidget();
	event_w->setEventId(eventName());
	event_w->setEventIdEditable(false);
	event_w->loadParams(eventConfig()->value("event").toMap());
	dlg.setCentralWidget(event_w);
	if(!dlg.exec())
		return;

	eventConfig()->setValue("event", event_w->saveParams());
	eventConfig()->save("event");
}

bool EventPlugin::closeEvent()
{
	qfLogFuncFrame();
	setEventName(QString());
	return true;
}

bool EventPlugin::openEvent(const QString &_event_name)
{
	closeEvent();
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
		QStringList event_names;
		{
			qb.select("nspname")
					.from("pg_catalog.pg_namespace  AS n")
					.where("nspname NOT LIKE 'pg\\_%'")
					.where("nspname NOT IN ('public', 'information_schema')")
					.orderBy("nspname");
			q.exec(qb.toString());
			while(q.next()) {
				event_names << q.value("nspname").toString();
			}
		}
		if(!event_names.contains(event_name))
			event_name = QString();
		ok = !event_name.isEmpty();
		if(!ok) {
			event_name = QInputDialog::getItem(fwk, tr("Query"), tr("Open event"), event_names, 0, false, &ok);
		}
		//Log.info(event_name, typeof event_name, (event_name)? "T": "F");
		ok = ok && !event_name.isEmpty();
		if(ok) {
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
			if(!QFile::exists(event_fn)) {
				event_fn = event_name = QString();
			}
		}
		if(event_name.isEmpty()) {
			QDir working_dir(connection_settings.singleWorkingDir());
			QStringList event_names = working_dir.entryList(QStringList() << ('*' + QBE_EXT), QDir::Files | QDir::Readable, QDir::Name);
			for (int i = 0; i < event_names.count(); ++i) {
				event_names[i] = fileNameToEventName(event_names[i]);
			}
			event_name = QInputDialog::getItem(fwk, tr("Query"), tr("Open event"), event_names, 0, false, &ok);
			//event_fn = qfd::FileDialog::getOpenFileName(fwk, tr("Select event"), connection_settings.singleWorkingDir(), tr("Quick Event files (*%1)").arg(qbe_ext));
			if(ok && !event_name.isEmpty()) {
				event_fn = eventNameToFileName(event_name);
			}
		}
		//Log.info(event_name, typeof event_name, (event_name)? "T": "F");
		{
			qfs::Connection conn(QSqlDatabase::database());
			conn.close();
			qfInfo() << "removing database:" << conn.connectionName();
			QSqlDatabase::removeDatabase(conn.connectionName());
			QSqlDatabase::addDatabase("QSQLITE");
		}
		if(event_fn.isEmpty()) {
			ok = false;
		}
		else {
			if(QFile::exists(event_fn)) {
				qfs::Connection conn(QSqlDatabase::database());
				conn.setDatabaseName(event_fn);
				qfInfo() << "Opening database file" << event_fn;
				if(conn.open()) {
					qfs::Query q(conn);
					ok = q.exec("PRAGMA foreign_keys=ON");
				}
				else {
					qfd::MessageBox::showError(fwk, tr("Open Database Error: %1").arg(conn.errorString()));
				}
			}
			else {
				qfd::MessageBox::showError(fwk, tr("Database file %1 doesn't exist.").arg(event_fn));
			}
		}
	}
	else {
		qfError() << "Invalid connection type:" << static_cast<int>(connection_type);
	}
	if(ok) {
		eventConfig(true);
		connection_settings.setEventName(event_name);
		setEventName(event_name);
		emit eventOpened(eventName());
		//emit reloadDataRequest();
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



