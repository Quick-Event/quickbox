#include "settingspersistenceattached.h"

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
		emit keyChanged(m_key);
	}
}

QString SettingsPersistenceAttached::settingsPersistencePath()
{
	QString ret = key();
	if(!ret.isEmpty()) {
		for(QObject *obj=this->parent(); obj!=nullptr; obj=obj->parent()) {
			SettingsPersistenceAttached *spa = qobject_cast<SettingsPersistenceAttached*>(qmlAttachedPropertiesObject<SettingsPersistence>(obj, false));
			if(spa) {
				QString s = spa->key();
				if(!s.isEmpty()) {
					if(!ret.isEmpty()) {
						ret = '/' + ret;
					}
					ret = s + ret;
				}
			}
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
