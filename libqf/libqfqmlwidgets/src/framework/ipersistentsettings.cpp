#include "ipersistentsettings.h"

#include <qf/core/assert.h>

#include <QMetaMethod>
//#include <QQmlProperty>

using namespace qf::qmlwidgets::framework;

IPersistentSettings::IPersistentSettings(QObject *controlled_object) :
	m_controlledObject(controlled_object)
{
	Q_ASSERT(m_controlledObject != nullptr);
}

QString IPersistentSettings::persistentSettingsId()
{
	return m_persistentSettingsId;
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
	qfLogFuncFrame() << persistentSettingsId() << m_controlledObject->property("persistentSettingsId").toString();
	QString ret = persistentSettingsId();
	if(!ret.isEmpty()) {
		for(QObject *obj=m_controlledObject->parent(); obj!=nullptr; obj=obj->parent()) {
			// reading property using QQmlProperty is crashing my app Qt 5.3.1 commit a83826dad0f62d7a96f5a6093240e4c8f7f2e06e
			//QQmlProperty p(obj, "persistentSettingsId");
			//QVariant v2 = p.read();
			QVariant vid = obj->property("persistentSettingsId");
			QString parent_id = vid.toString();
			if(vid.isValid() && parent_id.isEmpty()) {
				// property exists but is empty
				// don't know why, but I'm not able to read "persistentSettingsId" property of PartWidget here even if it is set
				// use objectName() fallback here
				parent_id = obj->objectName();
			}
			qfDebug() << "\t" << obj << "->" << vid.toString() << "type:" << vid.typeName() << "object name:" << obj->objectName() << "id:" << parent_id;
			//qfDebug() << "\tnebo ->" << v2.toString() << "type:" << v2.typeName();
			//IPersistentSettings *ip = dynamic_cast<IPersistentSettings*>(obj);
			//if(ip)
			//	qfDebug() << "\t\t" << ip << "->" << ip->persistentSettingsId();
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
