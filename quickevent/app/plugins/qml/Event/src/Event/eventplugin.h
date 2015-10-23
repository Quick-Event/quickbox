#ifndef EVENTPLUGIN_H
#define EVENTPLUGIN_H

#include "../eventpluginglobal.h"
#include "eventconfig.h"
#include "stage.h"

#include <qf/core/utils.h>
#include <qf/qmlwidgets/framework/plugin.h>

#include <QVariantMap>
#include <QSqlDriver>

namespace qf {
namespace core {
namespace sql {
class Query;
}
}
namespace qmlwidgets {
class Action;
namespace framework {
}
}
}

class QComboBox;
class DbSchema;

namespace Event {

class EVENTPLUGIN_DECL_EXPORT EventPlugin : public qf::qmlwidgets::framework::Plugin
{
	Q_OBJECT
	Q_PROPERTY(QObject* eventConfig READ eventConfig)
	Q_PROPERTY(int currentStageId READ currentStageId NOTIFY currentStageIdChanged)
	Q_PROPERTY(int stageCount READ stageCount)
	Q_PROPERTY(QString eventName READ eventName NOTIFY eventNameChanged)
	Q_PROPERTY(bool dbOpen READ isDbOpen NOTIFY dbOpenChanged)
private:
	typedef qf::qmlwidgets::framework::Plugin Super;
public:
	enum class ConnectionType : int {SqlServer = 0, SingleFile};
	EventPlugin(QObject *parent = nullptr);

	QF_PROPERTY_IMPL(QString, e, E, ventName)

	static const char *DBEVENT_NOTIFY_NAME;

	Q_INVOKABLE void initEventConfig();
	Event::EventConfig* eventConfig(bool reload = false);
	int stageCount();

	Q_SLOT void setCurrentStageId(int stage_id);
	int currentStageId();
	Q_SIGNAL void currentStageIdChanged(int current_stage);

	Q_INVOKABLE int stageStart(int stage_id);

	//Q_INVOKABLE QVariantMap stageDataMap(int stage_id) {return stageData(stage_id);}
	StageData stageData(int stage_id);
	Q_SLOT void clearStageDataCache();

	Q_SLOT bool createEvent(const QString &event_name = QString(), const QVariantMap &event_params = QVariantMap());
	Q_SLOT void editEvent();
	Q_SLOT bool closeEvent();
	Q_SLOT bool openEvent(const QString &event_name = QString());
	Q_SLOT void exportEvent();
	Q_SLOT void importEvent_qbe();

	Q_SIGNAL void reloadDataRequest();

	bool isDbOpen() const { return m_dbOpen; }
	Q_SIGNAL void dbOpenChanged(bool is_open);

	Q_SIGNAL void eventOpened(const QString &event_name);

	Q_INVOKABLE void emitDbEvent(const QString &domain, const QVariant &payload = QVariant(), bool loopback = true);
	Q_SIGNAL void dbEventNotify(const QString &domain, const QVariant &payload);

	Q_INVOKABLE QString classNameById(int class_id);

	DbSchema dbSchema();
public:
	// event wide signals
	//Q_SIGNAL void editStartListRequest(int stage_id, int class_id, int competitor_id);
private:
	void setDbOpen(bool ok);

	ConnectionType connectionType() const;
	QStringList existingSqlEventNames() const;
	QStringList existingFileEventNames(const QString &dir = QString()) const;

	Q_SLOT void onInstalled();
	Q_SLOT void onEventOpened();
	Q_SLOT void connectToSqlServer();
	Q_SLOT void onCbxStageActivated(int ix);
	Q_SLOT void loadCurrentStageId();
	Q_SLOT void saveCurrentStageId(int current_stage);
	Q_SLOT void editStage();
	Q_SLOT void onDbEvent(const QString & name, QSqlDriver::NotificationSource source, const QVariant & payload);

	//bool runSqlScript(qf::core::sql::Query &q, const QStringList &sql_lines);
private:
	qf::qmlwidgets::Action *m_actConnectDb = nullptr;
	qf::qmlwidgets::Action *m_actEvent = nullptr;
	qf::qmlwidgets::Action *m_actCreateEvent = nullptr;
	qf::qmlwidgets::Action *m_actOpenEvent = nullptr;
	qf::qmlwidgets::Action *m_actEditEvent = nullptr;
	qf::qmlwidgets::Action *m_actExportEvent = nullptr;
	qf::qmlwidgets::Action *m_actImportEvent = nullptr;
	qf::qmlwidgets::Action *m_actEditStage = nullptr;
	Event::EventConfig *m_eventConfig = nullptr;
	bool m_dbOpen = false;
	QComboBox *m_cbxStage = nullptr;
	QMap<int, StageData> m_stageCache;
	QMap<int, QString> m_classNameCache;
};

}

#endif // EVENTPLUGIN_H
