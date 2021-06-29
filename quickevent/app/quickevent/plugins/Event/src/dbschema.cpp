#include "dbschema.h"
#include "eventplugin.h"

//#include <qf/qmlwidgets/framework/mainwindow.h>
#include <qf/qmlwidgets/framework/application.h>

#include <qf/core/assert.h>
#include <qf/core/log.h>

#include <QQmlListProperty>
#include <QSqlRecord>
#include <QSqlField>
#include <QMetaObject>
#include <QQmlEngine>
#include <QQmlContext>
#include <QQmlComponent>

DbSchema::DbSchema(Event::EventPlugin *event_plugin)
	: QObject(event_plugin)
	, m_eventPlugin(event_plugin)
{
	QF_ASSERT_EX(m_eventPlugin != nullptr, "Bad plugin!");
}

QObject *DbSchema::dbSchemaRoot()
{
	qfLogFuncFrame();
	if(m_dbschemaComponent == nullptr) {
		auto *qml_engine = qmlEngine();
		//auto *app = qf::qmlwidgets::framework::Application::instance();
		QString dbschema_file = m_eventPlugin->qmlDir() + "/DbSchema.qml";
		m_dbschemaComponent = new QQmlComponent(qml_engine, qml_engine);
		QUrl dbschema_url = QUrl::fromLocalFile(dbschema_file);
		m_dbschemaComponent->loadUrl(dbschema_url, QQmlComponent::PreferSynchronous);
		QString errs = m_dbschemaComponent->errorString();
		if(errs.isEmpty()) {
			qfDebug() << "QQmlComponent is ready:" << m_dbschemaComponent->isReady();
			m_dbSchemaRoot = m_dbschemaComponent->beginCreate(qml_engine->rootContext());
			if(!m_dbSchemaRoot) {
				qfError() << "Error creating root object from component:" << m_dbschemaComponent << m_dbschemaComponent->url().toString();
				Q_FOREACH(auto err, m_dbschemaComponent->errors())
					qfError() << err.toString();
			}
			m_dbschemaComponent->completeCreate();
		}
		else {
			qfError() << "Load QML component error:" << errs;
			m_dbSchemaRoot = nullptr;
		}
	}
	return m_dbSchemaRoot;
}

QStringList DbSchema::createDbSqlScript(const DbSchema::CreateDbSqlScriptOptions &create_options)
{
	QObject *db_schema = dbSchemaRoot();
	if(!db_schema) {
		qfError() << "DbSchema QML object not created";
		return QStringList();
	}
	QVariant ret_val;
	QMetaObject::invokeMethod(db_schema, "createSqlScript", Qt::DirectConnection,
							  Q_RETURN_ARG(QVariant, ret_val),
							  Q_ARG(QVariant, create_options));
	QStringList ret = ret_val.toStringList();
	QF_CHECK(!ret.isEmpty(), "createDbSqlScript ERROR!");
	return ret;
}

QList<QObject *> DbSchema::tables()
{
	QList<QObject *> ret;
	QObject *db_schema = dbSchemaRoot();
	QQmlListReference tbllst(db_schema, "tables", qmlEngine());
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
	QQmlListReference fields(table, "fields", qmlEngine());
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

QQmlEngine *DbSchema::qmlEngine()
{
	return m_eventPlugin->qmlEngine();
}

