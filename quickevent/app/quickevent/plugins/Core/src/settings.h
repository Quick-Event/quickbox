#ifndef CORE_SETTINGS_H
#define CORE_SETTINGS_H

#include <QString>
#include <QSettings>

#define SETTINGS_QUOTE(x) #x

#define SETTINGS_FIELD(ptype, getter_prefix, setter_prefix, name_rest, path, default_value) \
	public: ptype getter_prefix##name_rest() const { return qvariant_cast<ptype>(value(path, default_value)); } \
	public: void setter_prefix##name_rest(const ptype &val) { setValue(path, val); }

#define SETTINGS_FIELD1(ptype, getter_prefix, setter_prefix, name_rest, default_value) \
	SETTINGS_FIELD(ptype, getter_prefix, setter_prefix, name_rest, SETTINGS_QUOTE(getter_prefix##name_rest), default_value)

namespace Core {

class Settings
{
public:
	Settings(const QString &settings_prefix);
protected:
	QVariant value(const QString &key, const QVariant &default_value = QVariant()) const;
	void setValue(const QString &key, const QVariant &value);
private:
	QSettings* appSettingsSingleton() const;
	QString fullPath(const QString &path) const;
private:
	QString m_settingsPrefix;
};

} // namespace Core

#endif // CORE_SETTINGS_H
