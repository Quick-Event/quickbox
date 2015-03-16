#include "query.h"
#include "querybuilder.h"

#include "../core/log.h"

#include <QSqlRecord>
#include <QSqlField>
#include <QVariant>

using namespace qf::core::sql;

Query::Query(const QSqlDatabase &db)
	: Super(db)
{
}

Query::Query(const QString &connection_name)
	: Super(QSqlDatabase::database(connection_name.isEmpty()? QLatin1String(QSqlDatabase::defaultConnection): connection_name, false))
{
}

bool Query::prepare(const QString &query, bool throw_exc)
{
	bool ret = Super::prepare(query);
	if(!ret && throw_exc) {
		QF_EXCEPTION(lastError().text());
	}
	return ret;
}

bool Query::exec(const QString &query, bool throw_exc)
{
	bool ret = Super::exec(query);
	if(!ret && throw_exc) {
		QF_EXCEPTION(lastError().text());
	}
	return ret;
}

bool Query::exec(bool throw_exc)
{
	bool ret = Super::exec();
	if(!ret && throw_exc) {
		QF_EXCEPTION(lastError().text());
	}
	return ret;
}

QSqlRecord Query::record() const
{
	QSqlRecord ret = Super::record();
	for(int i=0; i<ret.count(); i++) {
		QSqlField fld = ret.field(i);
		QString n = QueryBuilder::unmangleLongFieldName(fld.name());
		if(n != fld.name()) {
			fld.setName(n);
			ret.replace(i, fld);
		}
	}
	return ret;
}

int Query::fieldIndex(const QString &field_name) const
{
	QSqlRecord rc = record();
	int ix;
	for(ix=0; ix<rc.count(); ix++) {
		QString fld_mn = rc.fieldName(ix);
		if(fld_mn.endsWith(field_name, Qt::CaseInsensitive)) {
			if(fld_mn.length() == field_name.length())
				break;
			//qfWarning() << fld_mn << field_name << fld_mn.mid(0, fld_mn.length() - field_name.length());
			if(fld_mn[fld_mn.length() - field_name.length() - 1] == '.')
				break;
		}
	}
	if(ix >= rc.count())
		ix = -1;
	return ix;
}

QVariant Query::value(const QString &field_name) const
{
	QVariant ret;
	int ix = fieldIndex(field_name);
	if(ix < 0) {
		QSqlRecord rc = record();
		QStringList sl;
		for(ix=0; ix<rc.count(); ix++)
			sl << rc.fieldName(ix);
		qfError() << "Field" << field_name << "not found in the record!";
		qfInfo() << "Available fields:" << sl.join(", ");
	}
	else
		ret = value(ix);
	return ret;
}
