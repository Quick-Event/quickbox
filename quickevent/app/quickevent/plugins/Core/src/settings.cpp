#include "settings.h"

#include <QCoreApplication>

namespace Core {

Settings::Settings(const QString &settings_prefix)
	: m_settingsPrefix(settings_prefix)
{
}

QVariant Settings::value(const QString &key, const QVariant &default_value) const
{
	return appSettingsSingleton()->value(fullPath(key), default_value);
}

void Settings::setValue(const QString &key, const QVariant &value)
{
	appSettingsSingleton()->setValue(fullPath(key), value);
}

static const auto QE_APP_SETTINGS = QStringLiteral("qeAppSettings");

QSettings *Settings::appSettingsSingleton() const
{
	auto *settings = QCoreApplication::instance()->findChild<QSettings*>(QE_APP_SETTINGS, Qt::FindDirectChildrenOnly);
	if(!settings) {
		settings = new QSettings(QCoreApplication::instance());
		settings->setObjectName(QE_APP_SETTINGS);
	}
	return settings;
}

QString Settings::fullPath(const QString &path) const
{
	if(m_settingsPrefix.isEmpty())
		return path;
	return m_settingsPrefix + '/' + path;
}

} // namespace Core
