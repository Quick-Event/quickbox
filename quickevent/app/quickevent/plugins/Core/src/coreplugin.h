#pragma once

#include <qf/qmlwidgets/framework/plugin.h>

class QSettings;

namespace Core {

class CorePlugin : public qf::qmlwidgets::framework::Plugin
{
	Q_OBJECT
	using Super = qf::qmlwidgets::framework::Plugin;
public:
	CorePlugin(QObject *parent = nullptr);

	Q_INVOKABLE void launchSqlTool();
	Q_INVOKABLE void aboutQuickEvent();
	Q_INVOKABLE void aboutQt();

	static const QString SETTINGS_PREFIX_APPLICATION_LOCALE_LANGUAGE();

	void onInstalled();
};

}
