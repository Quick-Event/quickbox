#ifndef EVENTPLUGIN_H
#define EVENTPLUGIN_H

#include "eventpluginglobal.h"
#include "eventconfig.h"

#include <qf/core/utils.h>
#include <qf/qmlwidgets/framework/plugin.h>

namespace qf {
namespace qmlwidgets {
class Action;
namespace framework {
}
}
}

class QComboBox;

class EVENTPLUGIN_DECL_EXPORT EventPlugin : public qf::qmlwidgets::framework::Plugin
{
	Q_OBJECT
	Q_PROPERTY(QObject* eventConfig READ eventConfig)
	Q_PROPERTY(int currentStage READ currentStage NOTIFY currentStageChanged)
	Q_PROPERTY(int stageCount READ stageCount)
	Q_PROPERTY(QString eventName READ eventName NOTIFY eventNameChanged)
	Q_PROPERTY(bool dbOpen READ isDbOpen NOTIFY dbOpenChanged)
private:
	typedef qf::qmlwidgets::framework::Plugin Super;
public:
	enum class ConnectionType : int {SqlServer = 0, SingleFile};
	EventPlugin(QObject *parent = nullptr);

	Event::EventConfig* eventConfig(bool reload = false);
	int stageCount() {return eventConfig()->stageCount();}
	QString eventName() {return eventConfig()->eventName();}
	Q_SIGNAL void eventNameChanged(const QString &event_name);

	int currentStage();
	Q_SIGNAL void currentStageChanged(int current_stage);

	Q_SLOT bool createEvent(const QVariantMap &event_params = QVariantMap());
	Q_SLOT bool openEvent(const QString &event_name = QString());

	Q_SIGNAL void reloadDataRequest();
	Q_SIGNAL void eventOpened(const QString &event_name);

	bool isDbOpen() const { return m_dbOpen; }
	void setDbOpen(bool ok);
	Q_SIGNAL void dbOpenChanged(bool is_open);

	static int dlTest;
	Q_INVOKABLE int incDlTest() {return ++dlTest;}
private:
	Q_SLOT void onInstalled();
	Q_SLOT void onEventOpened();
	Q_SLOT void connectToSqlServer();
	Q_SLOT void onCbxStageActivated(int ix);
private:
	qf::qmlwidgets::Action *m_actConnectDb;
	qf::qmlwidgets::Action *m_actCreateEvent;
	qf::qmlwidgets::Action *m_actOpenEvent;
	Event::EventConfig *m_eventConfig = nullptr;
	bool m_dbOpen = false;
	QComboBox *m_cbxStage = nullptr;
};

#endif // EVENTPLUGIN_H
