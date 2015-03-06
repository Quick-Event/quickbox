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
	void onInstalled();
	void connectToSqlServer();
};

#endif // EVENTPLUGIN_H
