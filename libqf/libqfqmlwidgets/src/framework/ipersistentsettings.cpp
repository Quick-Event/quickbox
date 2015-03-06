#include "ipersistentsettings.h"

#include <qf/core/assert.h>

#include <QMetaMethod>
#include <QString>

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

void IPersistentSettings::setPersistentSettingsPath(const QString &path)
{
	m_path = path;
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

QString IPersistentSettings::persistentSettingsPathPrefix()
{
	static QString s("persistentSettings");
	return s;
}

QString IPersistentSettings::generatePersistentSettingsPath()
{
	qfLogFuncFrame() << persistentSettingsId() << m_controlledObject->property("persistentSettingsId").toString();
	QString generated_path = persistentSettingsId();
	QString ret;
	if(!generated_path.isEmpty()) {
		for(QObject *obj=m_controlledObject->parent(); obj!=nullptr; obj=obj->parent()) {
			IPersistentSettings *ps = dynamic_cast<IPersistentSettings*>(obj);
			if(ps) {
				ret = ps->persistentSettingsPath() + '/' + generated_path;
				//qfWarning() << "reading property 'persistentSettingsId' error" << obj << "casted to IPersistentSettings" << ps;
				//qfWarning() << "\tcorrect value should be:" << parent_id;
				break;
			}
			else {
				QVariant vid = obj->property("persistentSettingsId");
				QString id = vid.toString();
				if(!id.isEmpty()) {
					ret = id + '/' + generated_path;
				}
			}
			// reading property using QQmlProperty is crashing my app Qt 5.3.1 commit a83826dad0f62d7a96f5a6093240e4c8f7f2e06e
			//QQmlProperty p(obj, "persistentSettingsId");
			//QVariant v2 = p.read();
		}
		if(ret.isEmpty()) {
			ret = persistentSettingsPathPrefix() + '/' + generated_path;
		}
	}
	return ret;
}

