#ifndef QF_QMLWIDGETS_FRAMEWORK_IPERSISTENTSETTINGS_H
#define QF_QMLWIDGETS_FRAMEWORK_IPERSISTENTSETTINGS_H

class QString;
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
	void setPersistentSettingsKey(const QString &k);
	SettingsPersistenceAttached *settingPersistenceObject();
	QString settingsPersistencePath();
private:
	QObject *m_controlledObject;
};

}}}

#endif
