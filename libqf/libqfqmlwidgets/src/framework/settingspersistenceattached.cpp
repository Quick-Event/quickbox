#include "settingspersistenceattached.h"

#include <qf/core/log.h>

#include <QObject>
#include <QString>

using namespace qf::qmlwidgets::framework;

SettingsPersistenceAttached::SettingsPersistenceAttached(QObject *parent) :
	QObject(parent)
{
	connect(this, SIGNAL(keyChanged(QString)), parent, SLOT(loadPersistentSettings()));
}

void SettingsPersistenceAttached::setKey(const QString &k)
{
	if(k!= m_key) {
		m_key = k;
		m_path = generateSettingsPersistencePath();
		emit keyChanged(m_key);
	}
}

QString SettingsPersistenceAttached::settingsPersistencePath()
{
	return m_path;
}

QString SettingsPersistenceAttached::generateSettingsPersistencePath()
{
	qfLogFuncFrame();
	QString ret;
	if(!key().isEmpty()) {
		for(QObject *obj=this->parent(); obj!=nullptr; obj=obj->parent()) {
			//qfDebug() << "\t" << obj;
			SettingsPersistenceAttached *spa = obj->findChild<SettingsPersistenceAttached*>(QString(), Qt::FindDirectChildrenOnly);
			//SettingsPersistenceAttached *spa = qobject_cast<SettingsPersistenceAttached*>(qmlAttachedPropertiesObject<SettingsPersistence>(obj, false));
			//qfDebug() << "\t" << spa;
			if(spa) {
				//qfDebug() << "\t\t" << spa->key();
				QString s = spa->key();
				if(!s.isEmpty()) {
					if(!ret.isEmpty()) {
						ret = '/' + ret;
					}
					ret = s + ret;
				}
			}
			//qfDebug() << "\tRET:" << ret;
		}
	}
	if(!ret.isEmpty()) {
		static const QLatin1Literal PersistentSettings("persistentSettings");
		ret = PersistentSettings + '/' + ret;
	}
	return ret;
}


SettingsPersistenceAttached *SettingsPersistence::qmlAttachedProperties(QObject *object)
{
	return new SettingsPersistenceAttached(object);
}
