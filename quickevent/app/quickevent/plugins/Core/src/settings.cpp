#include "settings.h"

namespace Core {

Settings::Settings(const QString &settings_dir)
{
	m_settings.beginGroup(settings_dir);
}

QVariant Settings::value(const QString &key, const QVariant &default_value) const
{
	return m_settings.value(key, default_value);
}

void Settings::setValue(const QString &key, const QVariant &value)
{
	m_settings.setValue(key, value);
}

} // namespace Core
