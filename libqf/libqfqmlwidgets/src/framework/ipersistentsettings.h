#ifndef QF_QMLWIDGETS_FRAMEWORK_IPERSISTENTSETTINGS_H
#define QF_QMLWIDGETS_FRAMEWORK_IPERSISTENTSETTINGS_H

#include "../qmlwidgetsglobal.h"

#include <QString>

class QObject;

namespace qf {
namespace qmlwidgets {
namespace framework {

class QFQMLWIDGETS_DECL_EXPORT IPersistentSettings
{
public:
	virtual ~IPersistentSettings();
	IPersistentSettings(QObject *controlled_object);
public:
	virtual QString persistentSettingsId();
	virtual bool setPersistentSettingsId(const QString &id);

	virtual QString persistentSettingsPath();

	virtual void loadPersistentSettingsRecursively();
	//virtual void savePersistentSettingsRecursively();

	static const QString& defaultPersistentSettingsPathPrefix();
	QString persistentSettingsPathPrefix();
	void setPersistentSettingsPathPrefix(const QString &prefix);
protected:
	virtual QString effectivePersistentSettingsPathPrefix();
	virtual QString rawPersistentSettingsPath();
private:
	QObject *m_controlledObject;
	QString m_persistentSettingsId;
	QString m_persistentSettingsPathPrefix;
	QString m_persistentSettingsPath;
};

}}}

#endif
