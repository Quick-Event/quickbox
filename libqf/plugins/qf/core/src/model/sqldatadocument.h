#ifndef QF_CORE_QML_SQLDATADOCUMENT_H
#define QF_CORE_QML_SQLDATADOCUMENT_H

#include "../sql/sqlquerybuilder.h"

#include <qf/core/model/sqldatadocument.h>

namespace qf {
namespace core {
namespace qml {

//class SqlQueryBuilder;

class SqlDataDocument : public qf::core::model::SqlDataDocument
{
	Q_OBJECT
	Q_PROPERTY(qf::core::qml::SqlQueryBuilder* queryBuilder READ qmlSqlQueryBuilder)
private:
	typedef qf::core::model::SqlDataDocument Super;
public:
	SqlDataDocument(QObject *parent = nullptr);
	~SqlDataDocument() Q_DECL_OVERRIDE;
public:
	bool load() Q_DECL_OVERRIDE;
private:
	SqlQueryBuilder* qmlSqlQueryBuilder();
private:
	qf::core::qml::SqlQueryBuilder* m_qmlQueryBuilder = nullptr;
};

}}}

#endif // QF_CORE_QML_SQLDATADOCUMENT_H
