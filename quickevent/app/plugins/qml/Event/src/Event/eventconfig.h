#ifndef EVENTCONFIG_H
#define EVENTCONFIG_H

#include "../eventpluginglobal.h"

#include <QObject>
#include <QVariantMap>
#include <QSet>

namespace Event {

class EVENTPLUGIN_DECL_EXPORT EventConfig : public QObject
{
	Q_OBJECT
	Q_PROPERTY(int stageCount READ stageCount)
	//Q_PROPERTY(QString eventName READ eventName)
public:
	explicit EventConfig(QObject *parent = 0);
public slots:
	QVariantMap values() const {return m_data;}
	//void setValues(const QVariantMap &vals);
	QVariant value(const QStringList &path, const QVariant &default_value = QVariant()) const;
	QVariant value(const QString &path, const QVariant &default_value = QVariant()) const {return value(path.split('.'), default_value);}
	void setValue(const QStringList &path, const QVariant &val);
	void setValue(const QString &path, const QVariant &val) {setValue(path.split('.'), val);}
	void load();
	void save(const QString &path_to_save = QString());

	int stageCount() const;
	int currentStageId() const;
	int handicapLength() const;
	QString director() const;
	QString mainReferee() const;
	//QString eventName() const;
	//void setEventName(const QString &n);
private:
	//static const QSet<QString>& knownKeys();
	void save_helper(QVariantMap &ret, const QString &current_path, const QVariant &val);
	QVariantMap setValue_helper(const QVariantMap &m, const QStringList &path, const QVariant &val);
private:
	QVariantMap m_data;
};

}

#endif // EVENTCONFIG_H
