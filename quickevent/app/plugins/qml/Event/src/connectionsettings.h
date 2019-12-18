#ifndef CONNECTIONSETTINGS_H
#define CONNECTIONSETTINGS_H

#include "Event/eventplugin.h"

#include <qf/core/utils/settings.h>

class ConnectionSettings : public qf::core::utils::Settings
{
	Q_OBJECT
private:
	typedef qf::core::utils::Settings Super;
public:
	ConnectionSettings(QObject *parent = nullptr);
public:
	void setConnectionType(const Event::EventPlugin::ConnectionType &t);
	void setEventName(const QString &s);
	void setServerHost(const QString &s);
	void setServerPort(int p);
	void setServerUser(const QString &s);
	void setServerPassword(const QString &s);
	void setSingleWorkingDir(const QString &s);
public slots:
	Event::EventPlugin::ConnectionType connectionType();
	QString eventName();
	QString serverHost();
	int serverPort();
	QString serverUser();
	QString serverPassword();
	QString singleWorkingDir();
};

#endif // CONNECTIONSETTINGS_H
