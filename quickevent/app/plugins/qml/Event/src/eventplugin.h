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

class QE_EVENT_PLUGIN_DECL_EXPORT EventPlugin : public qf::qmlwidgets::framework::Plugin
{
	Q_OBJECT
	Q_PROPERTY(QObject* eventConfig READ eventConfig)
	Q_PROPERTY(int currentStage READ currentStage WRITE setCurrentStage NOTIFY currentStageChanged)
	Q_PROPERTY(int stageCount READ stageCount)
private:
	typedef qf::qmlwidgets::framework::Plugin Super;
public:
	enum class ConnectionType : int {SqlServer = 0, SingleFile};
	EventPlugin(QObject *parent = nullptr);

	QF_PROPERTY_IMPL2(int, c, C, urrentStage, 0)

	Event::EventConfig* eventConfig(bool reload = false);
	int stageCount() {return eventConfig()->stageCount();}
	//Q_INVOKABLE QString eventName();
private:
	Q_SLOT void onInstalled();
	Q_SLOT void connectToSqlServer();
	Q_SLOT bool createEvent(const QVariantMap &event_params = QVariantMap());
	Q_SLOT bool openEvent(const QString &event_name = QString());
private:
	qf::qmlwidgets::Action *m_actConnectDb;
	qf::qmlwidgets::Action *m_actCreateEvent;
	qf::qmlwidgets::Action *m_actOpenEvent;
	Event::EventConfig *m_eventConfig = nullptr;
};

#endif // EVENTPLUGIN_H
