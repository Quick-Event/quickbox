#include "sqlquerytablemodel.h"
#include "tablemodelcolumn.h"
#include "../sql/sqlquerybuilder.h"

#include <qf/core/assert.h>

#include <QQmlEngine>

using namespace qf::core::qml;

SqlQueryTableModel::SqlQueryTableModel(QObject *parent)
	: Super(parent), m_qmlQueryBuilder(nullptr)
{
}

SqlQueryTableModel::~SqlQueryTableModel()
{
	qDeleteAll(m_columns);
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
		QQmlEngine::setObjectOwnership(m_qmlQueryBuilder, QQmlEngine::CppOwnership);
	}
	return m_qmlQueryBuilder;
}

QQmlListProperty<TableModelColumn> SqlQueryTableModel::columns()
{
	return QQmlListProperty<TableModelColumn>(this,0,
                                    SqlQueryTableModel::addColumnFunction,
                                    SqlQueryTableModel::countColumnsFunction,
                                    SqlQueryTableModel::columnAtFunction,
                                    SqlQueryTableModel::removeAllColumnsFunction
                                    );
}

void SqlQueryTableModel::addColumnFunction(QQmlListProperty<TableModelColumn> *list_property, TableModelColumn *column)
{
	if (column) {
		SqlQueryTableModel *that = static_cast<SqlQueryTableModel*>(list_property->object);
		//qDebug() << "adding column" << column << column->parent();
		column->setParent(0);
		that->m_columns << column;
        that->insertColumn(that->m_columns.count(), column->columnDefinition());
    }
}

TableModelColumn *SqlQueryTableModel::columnAtFunction(QQmlListProperty<TableModelColumn> *list_property, int index)
{
	SqlQueryTableModel *that = static_cast<SqlQueryTableModel*>(list_property->object);
	return that->m_columns.value(index);
}

void SqlQueryTableModel::removeAllColumnsFunction(QQmlListProperty<TableModelColumn> *list_property)
{
	SqlQueryTableModel *that = static_cast<SqlQueryTableModel*>(list_property->object);
	while (that->columnCount()) {
		ColumnDefinition cd = that->removeColumn(0);
		QF_ASSERT(cd.isNull(), "Error removing column", break);
	}
	qDeleteAll(that->m_columns);
	that->m_columns.clear();
}

int SqlQueryTableModel::countColumnsFunction(QQmlListProperty<TableModelColumn> *list_property)
{
	SqlQueryTableModel *that = static_cast<SqlQueryTableModel*>(list_property->object);
	return that->m_columns.count();
}
