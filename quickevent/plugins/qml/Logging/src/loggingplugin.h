#ifndef LOGGINGPLUGIN_H
#define LOGGINGPLUGIN_H

#include <qf/qmlwidgets/framework/plugin.h>

class LoggingPlugin : public qf::qmlwidgets::framework::Plugin
{
	Q_OBJECT
	typedef qf::qmlwidgets::framework::Plugin Super;
public:
	LoggingPlugin(QObject *parent = nullptr);
private:
	Q_SLOT void onInstalled();
	Q_SLOT void saveSettings();
};

#endif // LOGGINGPLUGIN_H
