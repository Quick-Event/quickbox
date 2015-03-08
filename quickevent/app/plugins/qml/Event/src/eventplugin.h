#ifndef EVENTPLUGIN_H
#define EVENTPLUGIN_H

#include <qf/qmlwidgets/framework/plugin.h>

class EventPlugin : public qf::qmlwidgets::framework::Plugin
{
	Q_OBJECT
	typedef qf::qmlwidgets::framework::Plugin Super;
public:
	enum class ConnectionType : int {SqlServer = 0, SingleFile};
	EventPlugin(QObject *parent = nullptr);
private:
	Q_SLOT void onInstalled();
	Q_SLOT void connectToSqlServer();
	Q_SLOT bool openEvent(const QString &event_name = QString());
};

#endif // EVENTPLUGIN_H
