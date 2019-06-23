#include "eventplugin.h"
#include "../connectdbdialogwidget.h"
#include "../connectionsettings.h"
#include "../eventdialogwidget.h"
#include "../dbschema.h"
#include "stagedocument.h"
#include "stagewidget.h"

#include "../services/serviceswidget.h"
#include "../services/emmaclient.h"

#include <quickevent/core/og/timems.h>

#include <qf/qmlwidgets/framework/dockwidget.h>
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
#include <qf/core/assert.h>
#include <qf/core/sql/query.h>
#include <qf/core/sql/querybuilder.h>
#include <qf/core/sql/connection.h>
#include <qf/core/sql/transaction.h>
#include <qf/core/utils/fileutils.h>

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

namespace Event {

class DbEventPayload : public QVariantMap
{
private:
	typedef QVariantMap Super;

	QF_VARIANTMAP_FIELD(QString, e, setE, ventName)
	QF_VARIANTMAP_FIELD(QString, d, setD, omain)
	QF_VARIANTMAP_FIELD(int, c, setc, onnectionId)
	QF_VARIANTMAP_FIELD(QVariant, d, setD, ata)
public:
	DbEventPayload(const QVariantMap &data = QVariantMap()) : QVariantMap(data) {}

	static DbEventPayload fromJson(const QByteArray &json);

	QByteArray toJson() const;
};

DbEventPayload DbEventPayload::fromJson(const QByteArray &json)
{
	QJsonParseError error;
	QJsonDocument jsd = QJsonDocument::fromJson(json, &error);
	if(error.error == QJsonParseError::NoError) {
		QVariantMap m = jsd.toVariant().toMap();
		return DbEventPayload(m);
	}
	else {
		qfError() << "JSON parse error:" << error.errorString();
	}
	return DbEventPayload();
}

QByteArray DbEventPayload::toJson() const
{
	QJsonDocument jsd = QJsonDocument::fromVariant(*this);
	return jsd.toJson(QJsonDocument::Compact);
}

static auto QBE_EXT = QStringLiteral(".qbe");

const char* EventPlugin::DBEVENT_COMPETITOR_COUNTS_CHANGED = "competitorCountsChanged";
const char* EventPlugin::DBEVENT_CARD_READ = "cardRead";
//const char* EventPlugin::DBEVENT_CARD_CHECKED = "cardChecked";
const char* EventPlugin::DBEVENT_CARD_PROCESSED_AND_ASSIGNED = "cardProcessedAndAssigned";
const char* EventPlugin::DBEVENT_PUNCH_RECEIVED = "punchReceived";
const char* EventPlugin::DBEVENT_REGISTRATIONS_IMPORTED = "registrationsImported";
const char* EventPlugin::DBEVENT_STAGE_START_CHANGED = "stageStartChanged";

static QString singleFileStorageDir()
{
	ConnectionSettings connection_settings;
	QString ret = connection_settings.singleWorkingDir();
	return ret;
}

static QString eventNameToFileName(const QString &event_name)
{
	QString ret = singleFileStorageDir() + '/' + event_name + QBE_EXT;
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
	connect(this, &EventPlugin::eventNameChanged, [this](const QString &event_name) {
		setEventOpen(!event_name.isEmpty());
	});
	connect(this, &Event::EventPlugin::dbEventNotify, this, &Event::EventPlugin::onDbEventNotify, Qt::QueuedConnection);
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

int EventPlugin::stageIdForRun(int run_id)
{
	int ret = 0;
	qfs::QueryBuilder qb;
	qb.select2("runs", "stageId")
			.from("runs")
			.where("runs.id=" QF_IARG(run_id));
	qfs::Query q;
	q.exec(qb.toString(), qf::core::Exception::Throw);
	if(q.next())
		ret = q.value(0).toInt();
	else
		qfError() << "Cannot find runs record for id:" << run_id;
	return ret;
}

int EventPlugin::stageStartMsec(int stage_id)
{
	QTime start_time = stageStartTime(stage_id);
	int ret = start_time.msecsSinceStartOfDay();
	return ret;
}

QDate EventPlugin::stageStartDate(int stage_id)
{
	return stageStartDateTime(stage_id).date();
}

QTime EventPlugin::stageStartTime(int stage_id)
{
	return stageStartDateTime(stage_id).time();
}

QDateTime EventPlugin::stageStartDateTime(int stage_id)
{
	Event::StageData stage_data = stageData(stage_id);
	QDateTime dt = stage_data.startDateTime();
	return dt;
}

int EventPlugin::msecToStageStartAM(int si_am_time_sec, int msec, int stage_id)
{
	if(si_am_time_sec == 0xEEEE)
		return quickevent::core::og::LapTimeMs::UNREAL_TIME_MSEC;
	if(stage_id == 0)
		stage_id = currentStageId();
	int stage_start_msec = stageStartMsec(stage_id);
	int time_msec = quickevent::core::og::LapTimeMs::msecIntervalAM(stage_start_msec, si_am_time_sec * 1000 + msec);
	return time_msec;
}

void EventPlugin::setStageData(int stage_id, const QString &key, const QVariant &value)
{
	Event::StageDocument doc;
	doc.load(stage_id);
	doc.setValue(key, value);
	doc.save();
	clearStageDataCache();
}

StageData EventPlugin::stageData(int stage_id)
{
	QVariantMap ret;
	if(stage_id == 0)
		return ret;
	if(!m_stageCache.contains(stage_id)) {
		Event::StageDocument doc;
		doc.load(stage_id);
		if(doc.isEmpty()) {
			qfError() << "Cannot provide stage data for invalid stage id:" << stage_id;
			return ret;
		}
		StageData s(&doc);
		m_stageCache[stage_id] = s;
	}
	ret = m_stageCache.value(stage_id);
	return ret;
}

void EventPlugin::clearStageDataCache()
{
	qfInfo() << "stages data cache cleared";
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
	tb->setWindowTitle(tr("Event"));
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
	fwk->menuBar()->actionForPath("view/toolbar")->addActionInto(tb->toggleViewAction());

	services::EmmaClient *emma_client = new services::EmmaClient(this);
	services::Service::addService(emma_client);

	{
		m_servicesDockWidget = new qff::DockWidget(nullptr);
		m_servicesDockWidget->setObjectName("servicesDockWidget");
		m_servicesDockWidget->setWindowTitle(tr("Services"));
		fwk->addDockWidget(Qt::RightDockWidgetArea, m_servicesDockWidget);
		m_servicesDockWidget->hide();
		connect(m_servicesDockWidget, &qff::DockWidget::visibilityChanged, this, &EventPlugin::onServiceDockVisibleChanged);

		auto *a = m_servicesDockWidget->toggleViewAction();
		//a->setCheckable(true);
		//a->setShortcut(QKeySequence("ctrl+shift+R"));
		fwk->menuBar()->actionForPath("view")->addActionInto(a);
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
		emitDbEvent(Event::EventPlugin::DBEVENT_STAGE_START_CHANGED, stage_id, true);
	}
}

void EventPlugin::emitDbEvent(const QString &domain, const QVariant &data, bool loopback)
{
	qfLogFuncFrame() << "domain:" << domain << "payload:" << data;
	int connection_id = qf::core::sql::Connection::defaultConnection().connectionId();
	if(loopback) {
		// emit queued
		//emit dbEventNotify(domain, payload);
		QMetaObject::invokeMethod(this, "dbEventNotify", Qt::QueuedConnection,
								  Q_ARG(QString, domain),
								  Q_ARG(int, connection_id),
								  Q_ARG(QVariant, data));
	}
	if(connectionType() == ConnectionType::SingleFile)
		return;
	DbEventPayload dbpl;
	dbpl.setEventName(eventName());
	dbpl.setDomain(domain);
	dbpl.setData(data);
	dbpl.setconnectionId(connection_id);
	QByteArray json_ba = dbpl.toJson();
	QString payload_str = QString::fromUtf8(json_ba);
	if(payload_str.length() > 4000) {
		int len = payload_str.toUtf8().length();
		if(len > 8000) {
			qfInfo() << payload_str;
			qfError() << "Payload of size" << len << "is too long. Max Postgres payload length is 8000 bytes.";
			return;
		}
	}
	payload_str = qf::core::sql::Connection::escapeJsonForSql(payload_str);
	qf::core::sql::Connection conn = qf::core::sql::Connection::forName();
	QString qs = QString("NOTIFY ") + DBEVENT_NOTIFY_NAME + ", '" + payload_str + "'";
	qfDebug() << conn.driver() << "executing SQL:" << qs;
	QSqlQuery q(conn);
	if(!q.exec(qs)) {
		qfError() << "emitDbEventNotify Error:" << qs << q.lastError().text();
	}
}

QString EventPlugin::sqlDriverName()
{
	qf::core::sql::Connection cc = qf::core::sql::Connection::forName();
	return cc.driverName();
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

int EventPlugin::minDbVersion()
{
	qff::MainWindow *fwk = qff::MainWindow::frameWork();
	int db_version;
	QMetaObject::invokeMethod(fwk, "dbVersion", Qt::DirectConnection
							  , Q_RETURN_ARG(int, db_version));
	return db_version;
}

void EventPlugin::onDbEvent(const QString &name, QSqlDriver::NotificationSource source, const QVariant &payload)
{
	qfLogFuncFrame() << "name:" << name << "source:" << source << "payload:" << payload;
	if(name == QLatin1String(DBEVENT_NOTIFY_NAME)) {
		if(source == QSqlDriver::OtherSource) {
			DbEventPayload dbpl = DbEventPayload::fromJson(payload.toString().toUtf8());
			QString domain = dbpl.domain();
			if(domain.isEmpty()) {
				qfWarning() << "DbNotify with invalid domain, payload:" << payload.toString();
				return;
			}
			QString event_name = dbpl.eventName();
			if(event_name.isEmpty()) {
				qfWarning() << "DbNotify with invalid event name, payload:" << event_name << payload.toString();
				return;
			}
			if(event_name == eventName()) {
				QVariant data = dbpl.data();
				qfDebug() << "emitting domain:" << domain << "data:" << data;
				emit dbEventNotify(domain, dbpl.connectionId(), data);
			}
		}
		else {
			//qfDebug() << "self db notify";
		}
	}
}

void EventPlugin::onDbEventNotify(const QString &domain, int connection_id, const QVariant &data)
{
	Q_UNUSED(connection_id)
	Q_UNUSED(data)
	if(domain == QLatin1String(Event::EventPlugin::DBEVENT_STAGE_START_CHANGED)) {
		//int stage_id = data.toInt();
		clearStageDataCache();
	}
}

void EventPlugin::repairStageStarts(const qf::core::sql::Connection &from_conn, const qf::core::sql::Connection &to_conn)
{
	qfs::Query to_q(to_conn);
	qfs::Query from_q(from_conn);
	from_q.exec("SELECT * FROM stages ORDER BY id");
	while(from_q.next()) {
		int ix = from_q.fieldIndex(QStringLiteral("startDate"));
		if(ix < 0)
			break;
		QDate d = from_q.value(ix).toDate();
		QTime t = from_q.value("startTime").toTime();
		QDateTime dt(d, t);
		int id = from_q.value("id").toInt();
		to_q.exec("UPDATE stages SET startDateTime=" QF_SARG(dt.toString(Qt::ISODate)) " WHERE id=" QF_IARG(id));
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
				//qfInfo() << conn_w->serverPassword();
				db.setPassword(conn_w->serverPassword());
				db.setDatabaseName("quickevent");
				qfInfo().nospace() << "connecting to: " << db.userName() << "@" << db.hostName() << ":" << db.port();
				connect_ok = db.open();
				if(connect_ok) {
					bool ok = connect(db.driver(), SIGNAL(notification(QString, QSqlDriver::NotificationSource,QVariant)), this, SLOT(onDbEvent(QString,QSqlDriver::NotificationSource, QVariant)));
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
	setSqlServerConnected(connect_ok);
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
	QVariantMap replacements;
	replacements["minDbVersion"] = EventPlugin::minDbVersion();
	for(auto cmd : sql_lines) {
		if(cmd.isEmpty())
			continue;
		if(cmd.startsWith(QLatin1String("--")))
			continue;
		qfDebug() << cmd << ';';
		cmd = qf::core::Utils::replaceCaptions(cmd, replacements);
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

	closeEvent();

	Event::EventConfig event_config;
	bool ok = false;
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
		if(!conn.open()) {
			qfd::MessageBox::showError(fwk, tr("Open Database Error: %1").arg(conn.errorString()));
			return false;
		}
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

		qfInfo().nospace().noquote() << create_script.join(";\n") << ';';
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
			QDateTime start_dt = event_config.eventDateTime();
			// FIXME: handle SQL errors here and below in q.exec()
			q.prepare("INSERT INTO " + stage_table_name + " (id, startDateTime) VALUES (:id, :startDateTime)");
			for(int i=0; i<stage_count; i++) {
				q.bindValue(":id", i+1);
				q.bindValue(":startDateTime", start_dt);
				ok = q.exec();
				if(!ok) {
					break;
				}
				start_dt = start_dt.addDays(1);
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
	clearStageDataCache();
	m_classNameCache.clear();
	setEventName(QString());
	QF_SAFE_DELETE(m_eventConfig);
	//emit eventOpened(eventName()); //comment it till QE can load event with invalid name
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
		EventConfig *evc = eventConfig(true);
		if(evc->dbVersion() < minDbVersion()) {
			qfd::MessageBox::showError(fwk, tr("Event data version (%1) is too low, minimal version is (%2). Use Event/Import to convert event to current version.")
									   .arg(qf::core::Utils::intToVersionString(evc->dbVersion()))
									   .arg(qf::core::Utils::intToVersionString(minDbVersion())));
			closeEvent();
			return false;
		}
		if(evc->dbVersion() > minDbVersion()) {
			qfd::MessageBox::showError(fwk, tr("Event was created in more recent QuickEvent version (%1) and the application might not work as expected. Download latest QuickEvent is strongly recommended.")
									   .arg(qf::core::Utils::intToVersionString(evc->dbVersion())));
		}
		connection_settings.setEventName(event_name);
		setEventName(event_name);
		emit eventOpened(eventName());
		//emit reloadDataRequest();
	}
	return ok;
}

void EventPlugin::setSqlServerConnected(bool ok)
{
	if(ok != m_sqlServerConnected) {
		m_sqlServerConnected = ok;
		emit sqlServerConnectedChanged(ok);
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
	bool is_import_offrace = false;
	if(table_name == QLatin1String("runs")) {
		if(!src_rec.contains("isRunning") && dest_rec.contains("isRunning") && src_rec.contains("offRace")) {
			is_import_offrace = true;
			rec.append(dest_rec.field("isRunning"));
		}
	}
	auto *sqldrv = to_conn.driver();
	QString qs = sqldrv->sqlStatement(QSqlDriver::InsertStatement, table_name, rec, true);
	qfDebug() << qs;
	qfs::Query to_q(to_conn);
	if(!to_q.prepare(qs)) {
		QString ret = QString("Cannot prepare insert table SQL statement, table: %1.\n%2").arg(table_name).arg(to_q.lastErrorText());
		qfInfo() << qs;
		return ret;
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
			QVariant v;
			if((fld_name.compare(QLatin1String("isRunning"), Qt::CaseInsensitive) == 0) && is_import_offrace) {
				bool offrace = from_q.value(QStringLiteral("offRace")).toBool();
				v = offrace? QVariant(): QVariant(true);
			}
			else {
				v = from_q.value(fld_name);
				v.convert(static_cast<int>(rec.field(i).type()));
			}
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
	QString ex_fn = qf::qmlwidgets::dialogs::FileDialog::getSaveFileName (fwk, tr("Export as Quick Event"), singleFileStorageDir(), tr("Quick Event files *%1 (*%1)").arg(ext));
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

static QString fix_abs_times(qfs::Connection &from_conn, qfs::Connection &to_conn)
{
	try {
		int from_db_version = 0;
		{
			qfs::Query q(from_conn);
			q.execThrow("SELECT cvalue FROM config WHERE ckey='db.version'");
			if(q.next())
				from_db_version = q.value(0).toString().toInt();
		}
		/*
		int to_db_version = 0;
		{
			qfs::Query q(to_conn);
			q.execThrow("SELECT cvalue FROM config WHERE ckey='db.version'");
			if(q.next())
				to_db_version = q.value(0).toString().toInt();
		}
		*/
		if(from_db_version >= 10600)
			return QString(); // no changes needed
		qfInfo() << "Changint times to be relative to midnight";
		int stage_cnt = 0;
		{
			qfs::Query q(to_conn);
			q.execThrow("SELECT cvalue FROM config WHERE ckey='event.stageCount'");
			if(q.next())
				stage_cnt = q.value(0).toString().toInt();
		}
		if(stage_cnt <= 0)
			QF_EXCEPTION("Cannot read stage count");
		for (int stage_id = 1; stage_id <= stage_cnt; ++stage_id) {
			int start00_msec = -1;
			qfs::Query q(to_conn);
			q.execThrow("SELECT startDateTime FROM stages WHERE id=" QF_IARG(stage_id));
			if(q.next()) {
				QDateTime dt = q.value(0).toDateTime();
				start00_msec = dt.time().msecsSinceStartOfDay();
			}
			if(start00_msec <= 0)
				QF_EXCEPTION("Cannot read stage start time for stage: " + QString::number(stage_id));
			q.execThrow("UPDATE runs SET"
						" checkTimeMs=checkTimeMs+" QF_IARG(start00_msec)
						",startTimeMs=startTimeMs+" QF_IARG(start00_msec)
						",finishTimeMs=finishTimeMs+" QF_IARG(start00_msec)
						);
			q.execThrow("UPDATE runlaps SET"
						" stpTimeMs=stpTimeMs+" QF_IARG(start00_msec)
						);
			q.execThrow("UPDATE punches SET"
						" timeMs=timeMs+" QF_IARG(start00_msec)
						);
		}
	} catch (const std::exception &e) {
		QString err = QString::fromUtf8(e.what());
		return err;
	}
	return QString();
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
	QString fn = qf::qmlwidgets::dialogs::FileDialog::getOpenFileName (fwk, tr("Import as Quick Event"), QString(), tr("Quick Event files *%1 (*%1)").arg(ext));
	if(fn.isEmpty())
		return;
	QString event_name = qf::core::utils::FileUtils::baseName(fn) + "_2";
	event_name = QInputDialog::getText(fwk, tr("Query"), tr("Event will be imported as ID:"), QLineEdit::Normal, event_name).trimmed();
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
			if(table_name == QLatin1String("stages")) {
				repairStageStarts(imp_conn, exp_conn);
			}
		}
		if(!err_str.isEmpty())
			break;
		err_str = fix_abs_times(imp_conn, exp_conn);
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

void EventPlugin::onServiceDockVisibleChanged(bool on)
{
	if(on && !m_servicesDockWidget->widget()) {
		auto *rw = new services::ServicesWidget();
		m_servicesDockWidget->setWidget(rw);
		rw->reload();
	}
}

}

