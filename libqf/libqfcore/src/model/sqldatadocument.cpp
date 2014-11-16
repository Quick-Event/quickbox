#include "sqldatadocument.h"

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

