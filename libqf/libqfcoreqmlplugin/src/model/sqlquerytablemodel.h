#ifndef QF_CORE_QML_SQLQUERYTABLEMODEL_H
#define QF_CORE_QML_SQLQUERYTABLEMODEL_H

#include "tablemodelcolumn.h"

#include <qf/core/model/sqlquerytablemodel.h>

#include <QQmlListProperty>

namespace qf {
namespace core {
namespace qml {

class SqlQueryBuilder;
class TableModelColumn;

class SqlQueryTableModel : public qf::core::model::SqlQueryTableModel
{
	Q_OBJECT
	Q_PROPERTY(qf::core::qml::SqlQueryBuilder* queryBuilder READ qmlSqlQueryBuilder)
	Q_PROPERTY(QQmlListProperty<qf::core::qml::TableModelColumn> columns READ columns)
	Q_CLASSINFO("DefaultProperty", "columns")
private:
	typedef qf::core::model::SqlQueryTableModel Super;
public:
	explicit SqlQueryTableModel(QObject *parent = 0);
	~SqlQueryTableModel() Q_DECL_OVERRIDE;
protected:
	QString buildQuery() Q_DECL_OVERRIDE;
private:
	SqlQueryBuilder* qmlSqlQueryBuilder();

	QQmlListProperty<TableModelColumn> columns();
	static void addColumnFunction(QQmlListProperty<TableModelColumn> *list_property, TableModelColumn *column);
	static TableModelColumn* columnAtFunction(QQmlListProperty<TableModelColumn> *list_property, int index);
	static void removeAllColumnsFunction(QQmlListProperty<TableModelColumn> *list_property);
	static int countColumnsFunction(QQmlListProperty<TableModelColumn> *list_property);

private:
	qf::core::qml::SqlQueryBuilder* m_qmlQueryBuilder;
	QList<TableModelColumn*> m_columns;
};

}}}

#endif // QF_CORE_QML_SQLQUERYTABLEMODEL_H
