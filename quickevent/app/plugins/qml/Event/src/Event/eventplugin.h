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
namespace qmlwidgets {
class Action;
namespace framework {
}
}
}

class QComboBox;

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

	Event::EventConfig* eventConfig(bool reload = false);
	int stageCount() {return eventConfig()->stageCount();}

	int currentStageId();
	Q_SIGNAL void currentStageIdChanged(int current_stage);

	Stage stage(int stage_id);
	Q_SLOT void clearStageCache();

	Q_SLOT bool createEvent(const QString &_event_name = QString(), const QVariantMap &event_params = QVariantMap());
	Q_SLOT bool closeEvent();
	Q_SLOT bool openEvent(const QString &event_name = QString());

	Q_SIGNAL void reloadDataRequest();

	bool isDbOpen() const { return m_dbOpen; }
	Q_SIGNAL void dbOpenChanged(bool is_open);

	Q_SIGNAL void eventOpened(const QString &event_name);

	void emitDbEvent(const QString &domain, const QVariant &payload);
	Q_SIGNAL void dbEventNotify(const QString &domain, const QVariant &payload);
private:
	void setDbOpen(bool ok);

	Q_SLOT void onInstalled();
	Q_SLOT void onEventOpened();
	Q_SLOT void connectToSqlServer();
	Q_SLOT void onCbxStageActivated(int ix);
	Q_SLOT void editStage();
	Q_SLOT void onDbEvent(const QString & name, QSqlDriver::NotificationSource source, const QVariant & payload);
private:
	qf::qmlwidgets::Action *m_actConnectDb;
	qf::qmlwidgets::Action *m_actCreateEvent;
	qf::qmlwidgets::Action *m_actOpenEvent;
	qf::qmlwidgets::Action *m_actEditStage;
	Event::EventConfig *m_eventConfig = nullptr;
	bool m_dbOpen = false;
	QComboBox *m_cbxStage = nullptr;
	QMap<int, Stage> m_stageCache;
};

}

#endif // EVENTPLUGIN_H
