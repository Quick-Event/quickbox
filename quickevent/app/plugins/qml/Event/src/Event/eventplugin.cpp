#include "eventplugin.h"
#include "../connectdbdialogwidget.h"
#include "../connectionsettings.h"
#include "../eventdialogwidget.h"
#include "../dbschema.h"
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
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QComboBox>
#include <QLabel>
#include <QMetaObject>
#include <QSqlDriver>
#include <QJsonObject>
#include <QPushButton>
#include <QToolButton>
#include <QDirIterator>

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
	connect(this, &EventPlugin::currentStageIdChanged, this, &EventPlugin::saveCurrentStageId);
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

int EventPlugin::stageCount()
{
	if(eventName().isEmpty())
		return 0;
	return eventConfig()->stageCount();
}

void EventPlugin::setCurrentStageId(int stage_id)
{
	int ix = m_cbxStage->currentIndex();
	if(ix == stage_id-1)
		return;
	m_cbxStage->setCurrentIndex(stage_id - 1);
	emit currentStageIdChanged(stage_id);
}

int EventPlugin::currentStageId()
{
	return m_cbxStage->currentIndex() + 1;
}

int EventPlugin::stageStart(int stage_id)
{
	Event::StageData stage_data = stageData(stage_id);
	QTime start_time = stage_data.startTime();
	int ret = start_time.msecsSinceStartOfDay();
	return ret;
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

	m_actConnectDb = new qfw::Action(tr("&Connect to database"));
	//a->setShortcut("ctrl+L");
	connect(m_actConnectDb, SIGNAL(triggered()), this, SLOT(connectToSqlServer()));

	m_actOpenEvent = new qfw::Action(tr("&Open event"));
	//m_actOpenEvent->setShortcut("Ctrl+O");
	m_actOpenEvent->setEnabled(false);
	connect(m_actOpenEvent, SIGNAL(triggered()), this, SLOT(openEvent()));

	m_actCreateEvent = new qfw::Action(tr("Create eve&nt"));
	//m_actCreateEvent->setShortcut("Ctrl+N");
	m_actCreateEvent->setEnabled(false);
	connect(m_actCreateEvent, SIGNAL(triggered()), this, SLOT(createEvent()));

	m_actEditEvent = new qfw::Action(tr("E&dit event"));
	m_actEditEvent->setEnabled(false);
	connect(m_actEditEvent, SIGNAL(triggered()), this, SLOT(editEvent()));
	connect(this, &EventPlugin::eventNameChanged, [this](const QString &event_name) {
		this->m_actEditEvent->setEnabled(!event_name.isEmpty());
	});

	m_actExportEvent = new qfw::Action(tr("E&xport event"));
	m_actExportEvent->setEnabled(false);
	connect(m_actExportEvent, &QAction::triggered, this, &EventPlugin::exportEvent);

	m_actImportEvent = new qfw::Action(tr("I&mport event"));
	m_actImportEvent->setEnabled(false);
	connect(m_actImportEvent, &QAction::triggered, this, &EventPlugin::importEvent_qbe);

	connect(this, SIGNAL(eventNameChanged(QString)), fwk->statusBar(), SLOT(setEventName(QString)));
	connect(this, SIGNAL(currentStageIdChanged(int)), fwk->statusBar(), SLOT(setStageNo(int)));
	connect(fwk, &qff::MainWindow::pluginsLoaded, this, &EventPlugin::connectToSqlServer);
	connect(this, &EventPlugin::eventOpened, this, &EventPlugin::onEventOpened);

	qfw::Action *a_quit = fwk->menuBar()->actionForPath("file/import", false);
	a_quit->addActionBefore(m_actConnectDb);
	a_quit->addSeparatorBefore();

	m_actEvent = m_actConnectDb->addMenuAfter("file.event", tr("&Event"));

	m_actEvent->addActionInto(m_actCreateEvent);
	m_actEvent->addActionInto(m_actOpenEvent);
	m_actEvent->addActionInto(m_actEditEvent);
	m_actEvent->addActionInto(m_actExportEvent);
	m_actEvent->addActionInto(m_actImportEvent);

	qfw::ToolBar *tb = fwk->toolBar("Event", true);
	tb->setObjectName("EventToolbar");
	{
		QToolButton *bt_stage = new QToolButton();
		//bt_stage->setFlat(true);
		bt_stage->setAutoRaise(true);
		bt_stage->setCheckable(true);
		tb->addWidget(bt_stage);
		m_cbxStage = new QComboBox();
		connect(m_cbxStage, SIGNAL(activated(int)), this, SLOT(onCbxStageActivated(int)));
		connect(this, &EventPlugin::currentStageIdChanged, [bt_stage](int stage_id) {
			bt_stage->setText(tr("Current stage E%1").arg(stage_id));
		});
		QAction *act_stage = tb->addWidget(m_cbxStage);
		act_stage->setVisible(false);


		auto *style = qf::qmlwidgets::Style::instance();
		QIcon ico(style->icon("settings"));
		m_actEditStage = new qfw::Action(ico, "Stage settings");
		m_actEditStage->setVisible(false);
		connect(m_actEditStage, SIGNAL(triggered()), this, SLOT(editStage()));
		tb->addAction(m_actEditStage);

		connect(bt_stage, &QPushButton::clicked, [this, act_stage](bool checked) {
			act_stage->setVisible(checked);
			m_actEditStage->setVisible(checked);
		});
	}
}

void EventPlugin::onCbxStageActivated(int ix)
{
	emit this->currentStageIdChanged(ix + 1);
}

void EventPlugin::loadCurrentStageId()
{
	int stage_id = 0;
	if(!eventName().isEmpty())
		stage_id = eventConfig()->currentStageId();
	setCurrentStageId(stage_id);
}

void EventPlugin::saveCurrentStageId(int current_stage)
{
	if(current_stage != eventConfig()->currentStageId()) {
		eventConfig()->setValue("event.currentStageId", current_stage);
		eventConfig()->save("event");
	}
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

QString EventPlugin::classNameById(int class_id)
{
	if(m_classNameCache.isEmpty()) {
		qf::core::sql::Query q;
		q.exec("SELECT id, name FROM classes");
		while (q.next()) {
			m_classNameCache[q.value(0).toInt()] = q.value(1).toString();
		}
	}
	return m_classNameCache.value(class_id);
}

DbSchema EventPlugin::dbSchema()
{
	return DbSchema(this);
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
	m_cbxStage->setCurrentIndex(-1);
	m_cbxStage->blockSignals(false);
	loadCurrentStageId();
	//emit this->currentStageIdChanged(currentStageId());
}

EventPlugin::ConnectionType EventPlugin::connectionType() const
{
	ConnectionSettings connection_settings;
	return connection_settings.connectionType();
}

QStringList EventPlugin::existingSqlEventNames() const
{
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
	return event_names;
}

QStringList EventPlugin::existingFileEventNames(const QString &_dir) const
{
	/*
	QStringList ret;
	QDirIterator it(dir);
	while (it.hasNext()) {
		if(it.fileName().endsWith(QLatin1String(".qbe"), Qt::CaseInsensitive))
			ret << it.fileName().mid(0, it.fileName().length() - 4);
	}
	return ret;
	*/
	QString dir = _dir;
	if(dir.isEmpty()) {
		ConnectionSettings connection_settings;
		dir = connection_settings.singleWorkingDir();
	}
	QDir working_dir(dir);
	QStringList event_names = working_dir.entryList(QStringList() << ('*' + QBE_EXT), QDir::Files | QDir::Readable, QDir::Name);
	for (int i = 0; i < event_names.count(); ++i) {
		event_names[i] = fileNameToEventName(event_names[i]);
	}
	return event_names;
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
	m_actEditEvent->setEnabled(connect_ok);
	m_actExportEvent->setEnabled(connect_ok);
	m_actImportEvent->setEnabled(connect_ok);
	if(connect_ok) {
		openEvent(conn_w->eventName());
	}
}

static bool run_sql_script(qf::core::sql::Query &q, const QStringList &sql_lines)
{
	qfLogFuncFrame();
	for(auto cmd : sql_lines) {
		if(cmd.isEmpty())
			continue;
		if(cmd.startsWith(QLatin1String("--")))
			continue;
		qfDebug() << cmd << ';';
		bool ok = q.exec(cmd);
		if(!ok) {
			qfInfo() << cmd;
			qfError() << q.lastError().text();
			return false;
		}
	}
	return true;
}

bool EventPlugin::createEvent(const QString &event_name, const QVariantMap &event_params)
{
	qfLogFuncFrame();
	closeEvent();

	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	EventPlugin::ConnectionType connection_type = connectionType();
	QStringList existing_event_ids;
	if(connection_type == ConnectionType::SingleFile)
		existing_event_ids = existingFileEventNames();
	else
		existing_event_ids = existingSqlEventNames();
	QString event_id = event_name;
	QVariantMap new_params = event_params;
	do {
		qfd::Dialog dlg(fwk);
		dlg.setButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
		EventDialogWidget *event_w = new EventDialogWidget();
		event_w->setEventId(event_id);
		event_w->loadParams(new_params);
		dlg.setCentralWidget(event_w);
		if(!dlg.exec())
			return false;

		event_id = event_w->eventId();
		new_params = event_w->saveParams();
		if(event_id.isEmpty()) {
			qf::qmlwidgets::dialogs::MessageBox::showError(fwk, tr("Event ID cannot be empty."));
			continue;
		}
		if(existing_event_ids.contains(event_id)) {
			qf::qmlwidgets::dialogs::MessageBox::showError(fwk, tr("Event ID %1 exists already.").arg(event_id));
			continue;
		}
		break;
	} while(true);

	bool ok = false;
	Event::EventConfig event_config;
	//ConnectionSettings connection_settings;
	event_config.setValue("event", new_params);
	int stage_count = event_params.value("stageCount").toInt();
	if(stage_count == 0)
		stage_count = event_config.stageCount();
	qfInfo() << "createEvent, stage_count:" << stage_count;
	QF_ASSERT(stage_count > 0, "Stage count have to be greater than 0", return false);

	qfInfo() << "will create:" << event_id;
	qfs::Connection conn = qfs::Connection::forName();
	//QF_ASSERT(conn.isOpen(), "Connection is not open", return false);
	if(connection_type == ConnectionType::SingleFile) {
		QString event_fn = eventNameToFileName(event_id);
		conn.close();
		conn.setDatabaseName(event_fn);
		conn.open();
	}
	if(conn.isOpen()) {
		QVariantMap create_options;
		create_options["schemaName"] = event_id;
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
			ok = run_sql_script(q, create_script);
			if(!ok)
				break;
			qfDebug() << "creating stages:" << stage_count;
			QString stage_table_name = "stages";
			if(connection_type == ConnectionType::SqlServer)
				stage_table_name = event_id + '.' + stage_table_name;
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
			conn.setCurrentSchema(event_id);
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
		ok = openEvent(event_id);
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
	m_classNameCache.clear();
	setEventName(QString());
	QF_SAFE_DELETE(m_eventConfig);
	emit eventOpened(eventName()); //comment it till QE can load event with invalid name
	return true;
}

bool EventPlugin::openEvent(const QString &_event_name)
{
	closeEvent();
	//return true;
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	QString event_name = _event_name;
	ConnectionSettings connection_settings;
	ConnectionType connection_type = connection_settings.connectionType();
	//console.debug("openEvent()", "event_name:", event_name, "connection_type:", connection_type);
	bool ok = false;
	if(connection_type == ConnectionType::SqlServer) {
		//console.debug(db);
		QStringList event_names = existingSqlEventNames();
		if(!event_names.contains(event_name))
			event_name = QString();
		ok = !event_name.isEmpty();
		if(!ok) {
			event_name = QInputDialog::getItem(fwk, tr("Query"), tr("Open event"), event_names, 0, false, &ok);
		}
		//Log.info(event_name, typeof event_name, (event_name)? "T": "F");
		ok = ok && !event_name.isEmpty();
		if(ok) {
			qfs::Connection conn(QSqlDatabase::database());
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
			QStringList event_names = existingFileEventNames(connection_settings.singleWorkingDir());
			event_name = QInputDialog::getItem(fwk, tr("Query"), tr("Open event"), event_names, 0, false, &ok);
			//event_fn = qfd::FileDialog::getOpenFileName(fwk, tr("Select event"), connection_settings.singleWorkingDir(), tr("Quick Event files (*%1)").arg(qbe_ext));
			if(ok && !event_name.isEmpty()) {
				event_fn = eventNameToFileName(event_name);
			}
		}
		//Log.info(event_name, typeof event_name, (event_name)? "T": "F");
		{
			QString conn_name;
			{
				qfs::Connection conn(QSqlDatabase::database());
				conn_name = conn.connectionName();
				conn.close();
			}
			qfInfo() << "removing database:" << conn_name;
			QSqlDatabase::removeDatabase(conn_name);
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

static QString copy_sql_table(const QString &table_name, const QSqlRecord &dest_rec, qfs::Connection &from_conn, qfs::Connection &to_conn)
{
	qfLogFuncFrame() << table_name;
	qfInfo() << "Copying table:" << table_name;
	if(!to_conn.tableExists(table_name)) {
		qfWarning() << "Destination table" << table_name << "doesn't exist!";
		return QString();
	}
	qfs::Query from_q(from_conn);
	if(!from_q.exec(QString("SELECT * FROM %1").arg(table_name))) {
		qfWarning() << "Source table" << table_name << "doesn't exist!";
		return QString();
	}
	const QSqlRecord src_rec = from_q.record();
	// copy only fields which can be found in both records
	QSqlRecord rec;
	for (int i = 0; i < dest_rec.count(); ++i) {
		QString fld_name = dest_rec.fieldName(i);
		if(src_rec.indexOf(fld_name) >= 0) {
			qfDebug() << fld_name << "\t added to imported fields since it is present in both databases";
			rec.append(dest_rec.field(i));
		}
	}
	auto *sqldrv = to_conn.driver();
	QString qs = sqldrv->sqlStatement(QSqlDriver::InsertStatement, table_name, rec, true);
	qfDebug() << qs;
	qfs::Query to_q(to_conn);
	if(!to_q.prepare(qs)) {
		return QString("Cannot prepare insert table SQL statement, table: %1.").arg(table_name);
	}
	bool has_id_int = false;
	while(from_q.next()) {
		if(table_name == QLatin1String("config")) {
			if(from_q.value(0).toString() == QLatin1String("db.version"))
				continue;
		}
		for (int i = 0; i < rec.count(); ++i) {
			QSqlField fld = rec.field(i);
			QString fld_name = fld.name();
			//qfDebug() << "copy:" << fld_name << from_q.value(fld_name);
			QVariant v = from_q.value(fld_name);
			v.convert(rec.field(i).type());
			if(!has_id_int
					&& (fld.type() == QVariant::Int
						|| fld.type() == QVariant::UInt
						|| fld.type() == QVariant::LongLong
						|| fld.type() == QVariant::ULongLong)
					&& fld_name == QLatin1String("id")) {
				// probably ID INT AUTO_INCREMENT
				//max_id = qMax(max_id, v.toInt());
				has_id_int = true;
			}
			to_q.addBindValue(v);
		}
		if(!to_q.exec())
			return QString("SQL Error: %1").arg(to_q.lastError().text());
	}
	if(has_id_int && to_conn.driverName().endsWith(QLatin1String("PSQL"), Qt::CaseInsensitive)) {
		// set sequence current value when importing to PSQL
		qfInfo() << "updating seq number table:" << table_name;
		if(!to_q.exec("SELECT pg_catalog.setval(pg_get_serial_sequence(" QF_SARG(table_name) ", 'id'), MAX(id)) FROM " QF_CARG(table_name), !qf::core::Exception::Throw)) {
			return QString("Cannot update sequence counter, table: %1.").arg(table_name);
		}
	}
	return QString();
}

void EventPlugin::exportEvent()
{
	qfLogFuncFrame();
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	QString ext = ".qbe";
	QString ex_fn = qf::qmlwidgets::dialogs::FileDialog::getSaveFileName (fwk, tr("Export as Quick Event"), QString(), tr("Quick Event files *%1 (*%1)").arg(ext));
	if(ex_fn.isEmpty())
		return;
	if(!ex_fn.endsWith(ext, Qt::CaseInsensitive))
		ex_fn += ext;
	QString err_str;
	QString export_connection_name = QStringLiteral("qe_export_connection");
	do {
		if(QFile::exists(ex_fn)) {
			if(!QFile::remove(ex_fn)) {
				err_str = tr("Cannot delete existing file %1").arg(ex_fn);
				break;
			}
		}
		qfs::Connection ex_conn(QSqlDatabase::addDatabase("QSQLITE", export_connection_name));
		ex_conn.setDatabaseName(ex_fn);
		qfInfo() << "Opening export database file" << ex_fn;
		if(!ex_conn.open()) {
			qfd::MessageBox::showError(fwk, tr("Open Database Error: %1").arg(ex_conn.errorString()));
			return;
		}
		qfs::Transaction transaction(ex_conn);

		DbSchema db_schema = dbSchema();
		auto tables = db_schema.tables();
		int step_cnt = tables.count() + 1;
		int step_no = 0;
		fwk->showProgress(tr("Creating database"), ++step_no, step_cnt);
		{
			DbSchema::CreateDbSqlScriptOptions create_options;
			create_options.setDriverName(ex_conn.driverName());
			QStringList create_script = db_schema.createDbSqlScript(create_options);
			qfs::Query ex_q(ex_conn);
			if(!run_sql_script(ex_q, create_script)) {
				err_str = tr("Create Database Error: %1").arg(ex_q.lastError().text());
				break;
			}
		}
		qfs::Connection conn = qfs::Connection::forName();
		for(QObject *table : tables) {
			QString table_name = table->property("name").toString();
			qfDebug() << "Copying table" << table_name;
			fwk->showProgress(tr("Copying table %1").arg(table_name), ++step_no, step_cnt);
			QSqlRecord rec = db_schema.sqlRecord(table);
			err_str = copy_sql_table(table_name, rec, conn, ex_conn);
			if(!err_str.isEmpty())
				break;
		}
		if(!err_str.isEmpty())
			break;
		transaction.commit();
	} while(false);
	{
		QSqlDatabase c = QSqlDatabase::database(export_connection_name, false);
		if(c.isOpen())
			c.close();
	}
	QSqlDatabase::removeDatabase(export_connection_name);
	fwk->hideProgress();
	if(!err_str.isEmpty()) {
		qfd::MessageBox::showError(fwk, err_str);
	}
}

void EventPlugin::importEvent_qbe()
{
	qfLogFuncFrame();
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	/*
	if(connectionType() != ConnectionType::SqlServer) {
		qfd::MessageBox::showError(fwk, tr("Data file can be imported to SQL server only!"));
		return;
	}
	*/
	QString ext = ".qbe";
	QString fn = qf::qmlwidgets::dialogs::FileDialog::getOpenFileName (fwk, tr("Import as Quick Event"), QString(), "Quick Event files *.qbe (*.qbe)");
	if(fn.isEmpty())
		return;
	QString event_name = QInputDialog::getText(fwk, tr("Query"), tr("Event will be imported as ID:")).trimmed();
	if(event_name.isEmpty())
		return;
	QStringList existing_events = (connectionType() == ConnectionType::SingleFile)? existingFileEventNames(): existingSqlEventNames();
	if(existing_events.contains(event_name)) {
		qfd::MessageBox::showError(fwk, tr("Event ID '%1' exists already!").arg(event_name));
		return;
	}

	QString err_str;
	QString import_connection_name = QStringLiteral("qe_import_connection");
	QString export_connection_name = QStringLiteral("qe_export_connection");
	do {
		qfs::Connection current_conn = qfs::Connection::forName();

		qfs::Connection imp_conn(QSqlDatabase::addDatabase("QSQLITE", import_connection_name));
		imp_conn.setDatabaseName(fn);
		qfInfo() << "Opening import database file" << fn;
		if(!imp_conn.open()) {
			qfd::MessageBox::showError(fwk, tr("Open Database Error: %1").arg(imp_conn.errorString()));
			return;
		}

		qfs::Connection exp_conn(QSqlDatabase::addDatabase(current_conn.driverName(), export_connection_name));
		if(connectionType() == ConnectionType::SingleFile) {
			exp_conn.setDatabaseName(eventNameToFileName(event_name));
		}
		else {
			exp_conn.setHostName(current_conn.hostName());
			exp_conn.setPort(current_conn.port());
			exp_conn.setUserName(current_conn.userName());
			exp_conn.setPassword(current_conn.password());
			exp_conn.setDatabaseName(current_conn.databaseName());
		}
		qfInfo() << "Opening export database:" << exp_conn.databaseName();
		if(!exp_conn.open()) {
			qfd::MessageBox::showError(fwk, tr("Open Database Error: %1").arg(exp_conn.errorString()));
			return;
		}

		qfs::Transaction transaction(exp_conn);

		DbSchema db_schema = dbSchema();
		auto tables = db_schema.tables();
		int step_cnt = tables.count() + 1;
		int step_no = 0;
		fwk->showProgress(tr("Creating database"), ++step_no, step_cnt);
		{
			DbSchema::CreateDbSqlScriptOptions create_options;
			create_options.setDriverName(exp_conn.driverName());
			create_options.setSchemaName(event_name);
			QStringList create_script = db_schema.createDbSqlScript(create_options);
			qfs::Query ex_q(exp_conn);
			if(!run_sql_script(ex_q, create_script)) {
				err_str = tr("Create Database Error: %1").arg(ex_q.lastError().text());
				break;
			}
		}
		exp_conn.setCurrentSchema(event_name);
		for(QObject *table : tables) {
			QString table_name = table->property("name").toString();
			qfDebug() << "Copying table" << table_name;
			fwk->showProgress(tr("Copying table %1").arg(table_name), ++step_no, step_cnt);
			QSqlRecord rec = db_schema.sqlRecord(table, true);
			err_str = copy_sql_table(table_name, rec, imp_conn, exp_conn);
			if(!err_str.isEmpty())
				break;
		}
		if(!err_str.isEmpty())
			break;
		transaction.commit();
	} while(false);
	QSqlDatabase::removeDatabase(import_connection_name);
	QSqlDatabase::removeDatabase(export_connection_name);
	fwk->hideProgress();
	if(!err_str.isEmpty()) {
		qfd::MessageBox::showError(fwk, err_str);
		return;
	}
	if(qfd::MessageBox::askYesNo(fwk, tr("Open imported event '%1'?").arg(event_name))) {
		openEvent(event_name);
	}
}



