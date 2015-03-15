#ifndef EVENTCONFIG_H
#define EVENTCONFIG_H

#include "eventpluginglobal.h"

#include <QObject>
#include <QVariantMap>
#include <QSet>

namespace Event {

class EVENTPLUGIN_DECL_EXPORT EventConfig : public QObject
{
	Q_OBJECT
	Q_PROPERTY(int stageCount READ stageCount)
	Q_PROPERTY(QString eventName READ eventName)
public:
	explicit EventConfig(QObject *parent = 0);
public slots:
	QVariantMap values() const {return m_data;}
	void setValues(const QVariantMap &vals);
	QVariant value(const QString &key, const QVariant &default_value = QVariant()) const;
	void setValue(const QString &key, const QVariant &val);
	void load();
	void save(const QString &key_to_save = QString());

	int stageCount() const;
	QString eventName() const;
	void setEventName(const QString &n);
private:
	static const QSet<QString>& knownKeys();
private:
	QVariantMap m_data;
};

}

#endif // EVENTCONFIG_H
