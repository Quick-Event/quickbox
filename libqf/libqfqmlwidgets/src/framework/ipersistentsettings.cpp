#include "ipersistentsettings.h"

#include <qf/core/assert.h>

#include <QMetaMethod>

using namespace qf::qmlwidgets::framework;

IPersistentSettings::IPersistentSettings(QObject *controlled_object) :
	m_controlledObject(controlled_object)
{
	Q_ASSERT(m_controlledObject != nullptr);
}

void IPersistentSettings::setPersistentSettingsId(const QString &id)
{
	m_persistentSettingsId = id;
}

QString IPersistentSettings::persistentSettingsPath()
{
	if(!m_persistentSettingsId.isEmpty() && m_path.isEmpty()) {
		m_path = generatePersistentSettingsPath();
	}
	return m_path;
}

static void callMethodRecursively(QObject *obj, const char *method_name)
{
	if(!obj)
		return;
	int ix = obj->metaObject()->indexOfMethod(method_name);
	if(ix >= 0) {
		QMetaMethod mm = obj->metaObject()->method(ix);
		mm.invoke(obj);
	}
	for(auto o : obj->children()) {
		callMethodRecursively(o, method_name);
	}
}

void IPersistentSettings::loadPersistentSettingsRecursively()
{
	callMethodRecursively(m_controlledObject, "loadPersistentSettings()");
}

void IPersistentSettings::savePersistentSettingsRecursively()
{
	callMethodRecursively(m_controlledObject, "savePersistentSettings()");
}

QString IPersistentSettings::generatePersistentSettingsPath()
{
	qfLogFuncFrame();
	QString ret = persistentSettingsId();
	if(!ret.isEmpty()) {
		for(QObject *obj=m_controlledObject->parent(); obj!=nullptr; obj=obj->parent()) {
			QString parent_id = obj->property("persistentSettingsId").toString();
			if(!parent_id.isEmpty()) {
				ret = parent_id + '/' + ret;
			}
		}
	}
	if(!ret.isEmpty()) {
		static const QLatin1Literal PersistentSettings("persistentSettings");
		ret = PersistentSettings + '/' + ret;
	}
	return ret;
}
