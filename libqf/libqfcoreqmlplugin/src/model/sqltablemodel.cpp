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
	qDeleteAll(m_columns);
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
		that->m_columns << column;
		that->insertColumn(that->m_columns.count(), column->columnDefinition());
	}
}

TableModelColumn *SqlTableModel::columnAtFunction(QQmlListProperty<TableModelColumn> *list_property, int index)
{
	SqlTableModel *that = static_cast<SqlTableModel*>(list_property->object);
	return that->m_columns.value(index);
}

void SqlTableModel::removeAllColumnsFunction(QQmlListProperty<TableModelColumn> *list_property)
{
	qfLogFuncFrame();
	SqlTableModel *that = static_cast<SqlTableModel*>(list_property->object);
	while (that->columnCount()) {
		ColumnDefinition cd = that->removeColumn(0);
		QF_ASSERT(cd.isNull(), "Error removing column", break);
	}
	qDeleteAll(that->m_columns);
	that->m_columns.clear();
}

int SqlTableModel::countColumnsFunction(QQmlListProperty<TableModelColumn> *list_property)
{
	SqlTableModel *that = static_cast<SqlTableModel*>(list_property->object);
	return that->m_columns.count();
}
