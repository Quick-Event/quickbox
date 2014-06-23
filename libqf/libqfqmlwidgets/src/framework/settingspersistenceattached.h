#ifndef QF_QMLWIDGETS_FRAMEWORK_SETTINGSPERSISTENCEATTACHED_H
#define QF_QMLWIDGETS_FRAMEWORK_SETTINGSPERSISTENCEATTACHED_H

#include "../qmlwidgetsglobal.h"

#include <QString>
#include <qqml.h>

class QObject;

namespace qf {
namespace qmlwidgets {
namespace framework {

class QFQMLWIDGETS_DECL_EXPORT SettingsPersistenceAttached : public QObject
{
	Q_OBJECT
	Q_PROPERTY(QString key READ key WRITE setKey NOTIFY keyChanged)
public:
	SettingsPersistenceAttached(QObject *parent);
public:
	QString key() {return m_key;}
	void setKey(const QString &k);
	Q_SIGNAL void keyChanged(const QString &new_key);
	QString settingsPersistencePath();
private:
	QString m_key;
};

class QFQMLWIDGETS_DECL_EXPORT SettingsPersistence : public QObject
{
    Q_OBJECT
public:
    static SettingsPersistenceAttached *qmlAttachedProperties(QObject *object);
};

}}}

QML_DECLARE_TYPEINFO(qf::qmlwidgets::framework::SettingsPersistence, QML_HAS_ATTACHED_PROPERTIES)

#endif

