#include "sqlquery.h"
#include "sqlrecord.h"
#include "sqlquerybuilder.h"

#include <qf/core/log.h>

#include <QSqlError>

using namespace qf::core::qml;

SqlQuery::SqlQuery(QObject *parent) :
	QObject(parent), m_record(nullptr), m_queryBuilder(nullptr)
{
	qfLogFuncFrame() << this;
}

SqlQuery::~SqlQuery()
{
	qfLogFuncFrame() << this;
}

void SqlQuery::setDatabase(const QSqlDatabase &db)
{
	m_query = QSqlQuery(db);
}

bool SqlQuery::exec(const QString &query_str)
{
	qfLogFuncFrame() << query_str;
	bool ret;
	if(query_str.isEmpty())
		ret = m_query.exec();
	else
		ret = m_query.exec(query_str);
	QString err = lastError();
	if(!err.isEmpty()) {
		qfError() << "SQL ERROR:" << err;
	}
	return ret;
}

bool SqlQuery::exec(SqlQueryBuilder *qb)
{
	return exec(qb->toString());
}

QString SqlQuery::lastError()
{
	//qfLogFuncFrame();
	QString ret;
	QSqlError err = m_query.lastError();
	//qfDebug() << err.isValid() << err.databaseText();
	if(err.isValid()) {
		ret = err.text();
	}
	return ret;
}

QVariant SqlQuery::value(int ix)
{
	QVariant ret = m_query.value(ix);
	if(!ret.isValid()) {
		qfError() << "Field:" << ix << "not valit query record index, record count:" << record()->count();
	}
	return ret;
}

QVariant SqlQuery::value(const QString &field_name)
{
	QVariant ret = m_query.value(field_name);
	if(!ret.isValid()) {
		qfError() << "Field:" << field_name << "not found in query record, valid field names:" << record()->fieldNames();
	}
	return ret;
}

QVariantList SqlQuery::values()
{
	QVariantList ret;
	int cnt = m_query.record().count();
	for(int i=0; i<cnt; i++) {
		ret << value(i);
	}
	return ret;
}

SqlRecord *SqlQuery::record()
{
	if(!m_record)
		m_record = new SqlRecord(this);
	m_record->setRecord(m_query.record());
	return m_record;
}

SqlQueryBuilder *SqlQuery::builder()
{
	if(!m_queryBuilder) {
		m_queryBuilder = new SqlQueryBuilder(this);

	}
	return m_queryBuilder;
}

