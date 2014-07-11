#include "sqlquerytablemodel.h"
#include "sqlquerybuilder.h"

using namespace qf::core::qml;

SqlQueryTableModel::SqlQueryTableModel(QObject *parent)
	: Super(parent), m_qmlQueryBuilder(nullptr)
{
}

QString SqlQueryTableModel::buildQuery()
{
	if(m_qmlQueryBuilder) {
		m_queryBuilder =  m_qmlQueryBuilder->coreBuilder();
	}
	return Super::buildQuery();
}

SqlQueryBuilder *SqlQueryTableModel::qmlSqlQueryBuilder()
{
	if(!m_qmlQueryBuilder) {
		m_qmlQueryBuilder = new SqlQueryBuilder(this);
	}
	return m_qmlQueryBuilder;
}
