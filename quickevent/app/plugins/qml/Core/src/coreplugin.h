#ifndef COREPLUGIN_H
#define COREPLUGIN_H

#include <qf/qmlwidgets/framework/plugin.h>

class QSettings;

class CorePlugin : public qf::qmlwidgets::framework::Plugin
{
	Q_OBJECT
	typedef qf::qmlwidgets::framework::Plugin Super;
public:
	CorePlugin(QObject *parent = nullptr);

	Q_INVOKABLE void launchSqlTool();
	Q_INVOKABLE void aboutQuickEvent();
	Q_INVOKABLE void aboutQt();
private:
	Q_SLOT void onInstalled();
};

#endif // COREPLUGIN_H
