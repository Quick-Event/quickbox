#include "sqldatadocument.h"
#include "../core/assert.h"

using namespace qf::core::model;

SqlDataDocument::SqlDataDocument(QObject *parent)
	: Super(parent)
{

}

SqlDataDocument::~SqlDataDocument()
{

}

SqlTableModel *SqlDataDocument::model()
{
	return qobject_cast<SqlTableModel*>(Super::model());
}

qf::core::sql::QueryBuilder SqlDataDocument::queryBuilder()
{
	SqlTableModel *m = model();
	if(m)
		return m->queryBuilder();
	return qf::core::sql::QueryBuilder();
}

void SqlDataDocument::setQueryBuilder(const qf::core::sql::QueryBuilder &qb)
{
	SqlTableModel *m = model();
	QF_ASSERT(m != nullptr, "Document model is NULL", return);
	m->setQueryBuilder(qb);
}

SqlTableModel *SqlDataDocument::createModel(QObject *parent)
{
	return new SqlTableModel(parent);
}

bool SqlDataDocument::loadData()
{
	qfLogFuncFrame();
	bool ret = false;
	SqlTableModel *md = model();
	if(!md) {
		qfWarning() << "Model is NULL";
		return false;
	}
	if(!dataId().isValid()) {
		//if(mode() == ModeInsert) setDataId(-1);
		//setDataId(-1);
	}
	QVariant qp = md->queryParameters();
	if(qp.type() == QVariant::Map) {
		QVariantMap m = qp.toMap();
		m[idFieldName()] = dataId();
	}
	else {
		qp = dataId();
	}
	md->setQueryParameters(qp);
	md->reload();
	ret = Super::loadData();
	return ret;
}

