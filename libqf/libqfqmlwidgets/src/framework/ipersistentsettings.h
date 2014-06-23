#ifndef QF_QMLWIDGETS_FRAMEWORK_IPERSISTENTSETTINGS_H
#define QF_QMLWIDGETS_FRAMEWORK_IPERSISTENTSETTINGS_H

#include <QString>

class QObject;

namespace qf {
namespace qmlwidgets {
namespace framework {

class SettingsPersistenceAttached;

class IPersistentSettings
{
public:
	IPersistentSettings(QObject *controlled_object);
public:
	QString persistentSettingsId() {return m_persistentSettingsId;}
	void setPersistentSettingsId(const QString &id);
	QString persistentSettingsPath();
	void loadPersistentSettingsRecursively();
	void savePersistentSettingsRecursively();
private:
	QString generatePersistentSettingsPath();
private:
	QObject *m_controlledObject;
	QString m_persistentSettingsId;
	QString m_path;
};

}}}

#endif
