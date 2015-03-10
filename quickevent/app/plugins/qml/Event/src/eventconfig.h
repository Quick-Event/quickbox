#ifndef EVENTCONFIG_H
#define EVENTCONFIG_H

#include <QObject>
#include <QVariantMap>
#include <QSet>

class EventConfig : public QObject
{
	Q_OBJECT
public:
	explicit EventConfig(QObject *parent = 0);
public slots:
	QVariantMap values() {return m_data;}
	void setValues(const QVariantMap &vals);
	QVariant value(const QString &key, const QVariant &default_value = QVariant());
	void setValue(const QString &key, const QVariant &val);
	void load();
	void save();
private:
	static const QSet<QString>& knownKeys();
private:
	QVariantMap m_data;
};

#endif // EVENTCONFIG_H
