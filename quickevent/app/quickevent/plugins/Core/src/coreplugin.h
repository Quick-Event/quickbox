#pragma once

#include <qf/qmlwidgets/framework/plugin.h>

class QSettings;

namespace Core {

class SettingsDialog;

class CorePlugin : public qf::qmlwidgets::framework::Plugin
{
	Q_OBJECT
	using Super = qf::qmlwidgets::framework::Plugin;
public:
	CorePlugin(QObject *parent = nullptr);

	SettingsDialog* settingsDialog();

	Q_INVOKABLE void launchSqlTool();
	Q_INVOKABLE void aboutQuickEvent();
	Q_INVOKABLE void aboutQt();

	static const QString SETTINGS_PREFIX_APPLICATION_LOCALE_LANGUAGE();

	Q_REQUIRED_RESULT static QByteArray encrypt(const QByteArray &data, int min_length = 16);
	Q_REQUIRED_RESULT static QByteArray decrypt(const QByteArray &data);

	void onInstalled();	
private:
	SettingsDialog *m_settingsDialog = nullptr;
};

}
