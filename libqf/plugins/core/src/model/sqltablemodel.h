#ifndef QF_CORE_QML_SQLTABLEMODEL_H
#define QF_CORE_QML_SQLTABLEMODEL_H

#include "tablemodelcolumn.h"

#include <qf/core/model/sqltablemodel.h>

#include <QQmlListProperty>

namespace qf {
namespace core {
namespace qml {

class SqlQueryBuilder;
class TableModelColumn;

class SqlTableModel : public qf::core::model::SqlTableModel
{
	Q_OBJECT
	Q_PROPERTY(qf::core::qml::SqlQueryBuilder* queryBuilder READ sqlQueryBuilder /*NOTIFY queryBuilderChanged*/)
	Q_PROPERTY(QQmlListProperty<qf::core::qml::TableModelColumn> columns READ columns /*NOTIFY columnsChanged*/)
	Q_CLASSINFO("DefaultProperty", "columns")
private:
	typedef qf::core::model::SqlTableModel Super;
public:
	explicit SqlTableModel(QObject *parent = nullptr);
	~SqlTableModel() Q_DECL_OVERRIDE;

	Q_INVOKABLE bool reload() Q_DECL_OVERRIDE;

	Q_INVOKABLE qf::core::qml::SqlQueryBuilder* sqlQueryBuilder();
	Q_SIGNAL void queryBuilderChanged();

	//Q_SIGNAL void columnsChanged();
protected:
	QString buildQuery() Q_DECL_OVERRIDE;
private:

	void updateColumnDefinitionFromQml(int col_ix);

	void onQueryBuilderChanged();

	QQmlListProperty<TableModelColumn> columns();
	static void addColumnFunction(QQmlListProperty<TableModelColumn> *list_property, TableModelColumn *column);
	static TableModelColumn* columnAtFunction(QQmlListProperty<TableModelColumn> *list_property, int index);
	static void removeAllColumnsFunction(QQmlListProperty<TableModelColumn> *list_property);
	static int countColumnsFunction(QQmlListProperty<TableModelColumn> *list_property);

private:
	qf::core::qml::SqlQueryBuilder* m_qmlQueryBuilder = nullptr;
	QList<TableModelColumn*> m_qmlColumns;
};

}}}

#endif // QF_CORE_QML_SQLTABLEMODEL_H
