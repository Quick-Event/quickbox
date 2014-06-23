#include "ipersistentsettings.h"
#include "settingspersistenceattached.h"

#include <qf/core/assert.h>

using namespace qf::qmlwidgets::framework;

IPersistentSettings::IPersistentSettings(QObject *controlled_object) :
	m_controlledObject(controlled_object)
{
	Q_ASSERT(m_controlledObject != nullptr);
}

void IPersistentSettings::setPersistentSettingsKey(const QString &k)
{
	SettingsPersistenceAttached *spo = qobject_cast<SettingsPersistenceAttached*>(qmlAttachedPropertiesObject<SettingsPersistence>(m_controlledObject, true));
	QF_ASSERT(spo != nullptr, "internal error", return);
	spo->setKey(k);
}

SettingsPersistenceAttached *IPersistentSettings::settingPersistenceObject()
{
	SettingsPersistenceAttached *ret = qobject_cast<SettingsPersistenceAttached*>(qmlAttachedPropertiesObject<SettingsPersistence>(m_controlledObject, false));
	return ret;
}

QString IPersistentSettings::settingsPersistencePath()
{
	QString ret;
	SettingsPersistenceAttached *sp = settingPersistenceObject();
	if(sp) {
		ret = sp->settingsPersistencePath();
	}
	return ret;
}
