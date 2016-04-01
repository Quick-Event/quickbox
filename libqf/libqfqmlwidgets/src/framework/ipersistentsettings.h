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
	IPersistentSettings(QObject *controlled_object);
public:
	static QString persistentSettingsPathPrefix();

	virtual QString persistentSettingsId();
	virtual void setPersistentSettingsId(const QString &id);

	virtual QString persistentSettingsPath();
	virtual void setPersistentSettingsPath(const QString &path);

	virtual void loadPersistentSettingsRecursively();
	virtual void savePersistentSettingsRecursively();
private:
	QString generatePersistentSettingsPath();
private:
	QObject *m_controlledObject;
	QString m_persistentSettingsId;
	QString m_path;
};

}}}

#endif
