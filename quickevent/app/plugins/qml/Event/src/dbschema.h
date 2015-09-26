#ifndef DBSCHEMA_H
#define DBSCHEMA_H

#include <qf/core/utils.h>

#include <QList>
#include <QVariantMap>

namespace Event {
class EventPlugin;
}

class QObject;
class QSqlRecord;

class DbSchema
{
public:
	class CreateDbSqlScriptOptions : public QVariantMap
	{
	public:
		CreateDbSqlScriptOptions(const QVariantMap &o = QVariantMap()) : QVariantMap(o) {}

		QF_VARIANTMAP_FIELD(QString, s, setS, chemaName)
		QF_VARIANTMAP_FIELD(QString, d, setD, riverName)
	};
public:
	DbSchema(Event::EventPlugin *event_plugin);

	QObject* dbSchema();
	QStringList createDbSqlScript(const CreateDbSqlScriptOptions &create_options);
	QList<QObject*> tables();
	QObject* table(const QString &table_name);
	QSqlRecord sqlRecord(QObject *table, bool lowercase_field_names = false);
private:
	Event::EventPlugin *m_eventPlugin;
};

#endif // DBSCHEMA_H
