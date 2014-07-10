#include "sqlquerytablemodel.h"
#include "sqlquerybuilder.h"

using namespace qf::core::qml;

SqlQueryTableModel::SqlQueryTableModel(QObject *parent)
	: Super(parent), m_sqlQueryBuilder(nullptr)
{
}

SqlQueryBuilder *SqlQueryTableModel::qmlSqlQueryBuilder()
{
	if(!m_sqlQueryBuilder) {
		m_sqlQueryBuilder = new SqlQueryBuilder(this);
	}
	return m_sqlQueryBuilder;
}
