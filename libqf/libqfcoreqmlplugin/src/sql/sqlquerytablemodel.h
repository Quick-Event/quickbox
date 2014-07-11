#ifndef QF_CORE_QML_SQLQUERYTABLEMODEL_H
#define QF_CORE_QML_SQLQUERYTABLEMODEL_H

#include <qf/core/model/sqlquerytablemodel.h>

namespace qf {
namespace core {
namespace qml {

class SqlQueryBuilder;

class SqlQueryTableModel : public qf::core::model::SqlQueryTableModel
{
	Q_OBJECT
	Q_PROPERTY(qf::core::qml::SqlQueryBuilder* queryBuilder READ qmlSqlQueryBuilder)
private:
	typedef qf::core::model::SqlQueryTableModel Super;
public:
	explicit SqlQueryTableModel(QObject *parent = 0);
protected:
	QString buildQuery() Q_DECL_OVERRIDE;
private:
	SqlQueryBuilder* qmlSqlQueryBuilder();
private:
	qf::core::qml::SqlQueryBuilder* m_qmlQueryBuilder;
};

}}}

#endif // QF_CORE_QML_SQLQUERYTABLEMODEL_H
