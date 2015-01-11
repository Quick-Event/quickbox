#include "sqltablemodel.h"
#include "tablemodelcolumn.h"
#include "../sql/sqlquerybuilder.h"

#include <qf/core/assert.h>

#include <QQmlEngine>

using namespace qf::core::qml;

SqlTableModel::SqlTableModel(QObject *parent)
	: Super(parent), m_qmlQueryBuilder(nullptr)
{
	qfLogFuncFrame() << this;
}

SqlTableModel::~SqlTableModel()
{
	qfLogFuncFrame() << this;
	qDeleteAll(m_qmlColumns);
}

QString SqlTableModel::buildQuery()
{
	if(m_qmlQueryBuilder) {
		m_queryBuilder =  m_qmlQueryBuilder->coreBuilder();
	}
	return Super::buildQuery();
}

SqlQueryBuilder *SqlTableModel::qmlSqlQueryBuilder()
{
	if(!m_qmlQueryBuilder) {
		m_qmlQueryBuilder = new SqlQueryBuilder(this);
		QQmlEngine::setObjectOwnership(m_qmlQueryBuilder, QQmlEngine::CppOwnership);
	}
	return m_qmlQueryBuilder;
}

void SqlTableModel::updateColumnDefinitionFromQml(int col_ix)
{
	TableModelColumn *tc = m_qmlColumns.at(col_ix);
	m_columns[col_ix] = tc->columnDefinition();
}

QQmlListProperty<TableModelColumn> SqlTableModel::columns()
{
	return QQmlListProperty<TableModelColumn>(this,0,
											  SqlTableModel::addColumnFunction,
											  SqlTableModel::countColumnsFunction,
											  SqlTableModel::columnAtFunction,
											  SqlTableModel::removeAllColumnsFunction
											  );
}

void SqlTableModel::addColumnFunction(QQmlListProperty<TableModelColumn> *list_property, TableModelColumn *column)
{
	if (column) {
		SqlTableModel *that = static_cast<SqlTableModel*>(list_property->object);
		if(!column->parent()) {
			qfWarning() << "Every object that is not garbage collected by qml engine should have parent, reparenting column to model.";
			column->setParent(that);
		}
		that->m_qmlColumns << column;
		//qfInfo() << "############### adding column:" << column->columnDefinition().fieldName();
		int col_ix = that->columnCount();
		column->setColumnIndex(col_ix);
		that->insertColumn(that->m_qmlColumns.count(), column->columnDefinition());
		connect(column, &TableModelColumn::updated, that, &SqlTableModel::updateColumnDefinitionFromQml);
	}
}

TableModelColumn *SqlTableModel::columnAtFunction(QQmlListProperty<TableModelColumn> *list_property, int index)
{
	SqlTableModel *that = static_cast<SqlTableModel*>(list_property->object);
	return that->m_qmlColumns.value(index);
}

void SqlTableModel::removeAllColumnsFunction(QQmlListProperty<TableModelColumn> *list_property)
{
	qfLogFuncFrame();
	SqlTableModel *that = static_cast<SqlTableModel*>(list_property->object);
	while (that->columnCount()) {
		ColumnDefinition cd = that->removeColumn(0);
		QF_ASSERT(cd.isNull(), "Error removing column", break);
	}
	qDeleteAll(that->m_qmlColumns);
	that->m_qmlColumns.clear();
}

int SqlTableModel::countColumnsFunction(QQmlListProperty<TableModelColumn> *list_property)
{
	SqlTableModel *that = static_cast<SqlTableModel*>(list_property->object);
	return that->m_qmlColumns.count();
}
