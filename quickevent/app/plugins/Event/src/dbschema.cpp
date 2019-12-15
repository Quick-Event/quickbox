#include "dbschema.h"
#include "eventplugin.h"

#include <qf/core/assert.h>
#include <qf/core/log.h>

#include <QQmlListProperty>
#include <QSqlRecord>
#include <QSqlField>
#include <QMetaObject>

DbSchema::DbSchema(Event::EventPlugin *event_plugin)
	: m_eventPlugin(event_plugin)
{
	QF_ASSERT_EX(m_eventPlugin != nullptr, "Bad plugin!");
}

QObject *DbSchema::dbSchema()
{
	QObject *db_schema = nullptr;
	{
		QVariant ret_val;
		if(!QMetaObject::invokeMethod(m_eventPlugin, "dbSchema", Qt::DirectConnection, Q_RETURN_ARG(QVariant, ret_val))) {
			qfError() << "Internal error: Cannot get db schema";
			return nullptr;
		}
		db_schema = ret_val.value<QObject*>();
		QF_ASSERT(db_schema != nullptr, "Internal error: Cannot get db schema", return nullptr);
	}
	return db_schema;
}

QStringList DbSchema::createDbSqlScript(const DbSchema::CreateDbSqlScriptOptions &create_options)
{
	QVariant ret_val;
	QMetaObject::invokeMethod(m_eventPlugin, "createDbSqlScript", Qt::DirectConnection,
							  Q_RETURN_ARG(QVariant, ret_val),
							  Q_ARG(QVariant, create_options));
	QStringList ret = ret_val.toStringList();
	QF_CHECK(!ret.isEmpty(), "createDbSqlScript ERROR!");
	return ret;
}

QList<QObject *> DbSchema::tables()
{
	QList<QObject *> ret;
	QObject *db_schema = dbSchema();
	QQmlListReference tbllst(db_schema, "tables", m_eventPlugin->qmlEngine());
	for (int i = 0; i < tbllst.count(); ++i) {
		ret << tbllst.at(i);
	}
	return ret;
}

QObject *DbSchema::table(const QString &table_name)
{
	for(QObject *t : tables()) {
		if(t->property("name") == table_name)
			return t;
	}
	return nullptr;
}

QSqlRecord DbSchema::sqlRecord(QObject *table, bool lowercase_field_names)
{
	QSqlRecord ret;
	QQmlListReference fields(table, "fields", m_eventPlugin->qmlEngine());
	for (int i = 0; i < fields.count(); ++i) {
		QObject *field = fields.at(i);
		QString name = field->property("name").toString();
		if(lowercase_field_names)
			name = name.toLower();
		QVariant typev = field->property("type");
		QObject *type = typev.value<QObject*>();
		QF_ASSERT(type != nullptr, "Internal error: Cannot get field type", return QSqlRecord());
		QByteArray type_name = type->property("metaTypeName").toString().toLatin1();
		QSqlField fld(name, QVariant::nameToType(type_name.constData()));
		ret.append(fld);
		//qfInfo() << type << "name:" << name << "type:" << type_name;
	}
	return ret;
}

